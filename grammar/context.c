/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: context.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "context.h"
#include "avl-tree.h"
#include "stack.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"
#include "trie.h"
#include <stdint.h>

typedef struct GContext {
  const Allocator *allocator;
  Array /*<Register>*/ *regArray;
  Array /*<Immediate>*/ *immArray;
  Array /*<Memory>*/ *memArray;
  Array /*<Set>*/ *setArray;
  Array /*<RegisterGroup>*/ *grpArray;
  Array /*<Record>*/ *recordArray;
  Trie /*<uint32_t>*/ *objectMap;
  Trie /*<uint32_t>*/ *opcodeMap;
  codegen_t *(*getCodegen)(uint32_t token_type);

  // temporary variable
  Array *patterns;
  Stack *widthStack;
  Stack *identStack;
  AVLTree *mappingTree;
  Array *outputs;
} GContext;

inline GContext *GContext_new(const Allocator *allocator) {
  GContext *context = allocator->calloc(1, sizeof(GContext));
  context->allocator = allocator;
  context->regArray = Array_new(sizeof(Register), enum_Register, allocator);
  context->immArray = Array_new(sizeof(Immediate), enum_Immediate, allocator);
  context->memArray = Array_new(sizeof(Memory), enum_Memory, allocator);
  context->setArray = Array_new(sizeof(Set), enum_Set, allocator);
  context->grpArray = Array_new(sizeof(RegisterGroup), enum_RegisterGroup, allocator);
  context->recordArray = Array_new(sizeof(Record), INT32_MAX - 1, allocator);
  context->objectMap = Trie_new(allocator);
  context->opcodeMap = Trie_new(allocator);
  context->outputs = Array_new(sizeof(char_t), INT32_MAX, allocator);
  context->widthStack = Stack_new(allocator);
  context->identStack = Stack_new(allocator);
  context->mappingTree = nullptr;
  return context;
}

#define contextReleaseStack(stack)            \
  do {                                        \
    Stack_clear(context->stack);              \
    context->allocator->free(context->stack); \
  } while (false)
#define contextReleaseArray(array, destructor)              \
  do {                                                      \
    Array_reset(context->array, (destruct_t *) destructor); \
    Array_destroy(context->array);                          \
  } while (false)

inline void GContext_destroy(GContext *context) {
  if (context->mappingTree) { AVLTree_destroy(context->mappingTree, nullptr); }
  if (context->patterns) { releasePrimeArray(context->patterns); }
  contextReleaseArray(regArray, releaseRegister);
  contextReleaseArray(immArray, releaseImmediate);
  contextReleaseArray(memArray, releaseMemory);
  contextReleaseArray(setArray, releaseSet);
  contextReleaseArray(grpArray, releaseRegisterGroup);
  releasePrimeArray(context->recordArray);
  Trie_destroy(context->objectMap);
  Trie_destroy(context->opcodeMap);
  contextReleaseStack(widthStack);
  contextReleaseStack(identStack);
  context->allocator->free(context);
}

inline const Allocator *GContext_getAllocator(GContext *context) {
  return context->allocator;
}

inline Array *GContext_getOutputBuffer(GContext *context) {
  return context->outputs;
}

inline void GContext_setCodegen(GContext *context, codegen_t *(*getCodegen)(uint32_t token_type)) {
  context->getCodegen = getCodegen;
}

inline codegen_t *GContext_getCodegen(GContext *context, uint32_t token_type) {
  if (context->getCodegen) { return context->getCodegen(token_type); }
  return nullptr;
}

inline void GContext_addOpcode(GContext *context, const Identifier *ident, Instruction *instr) {
  Trie_set(context->opcodeMap, ident->ptr, instr);
}
inline void *GContext_findOpcode(GContext *context, const Identifier *ident) {
  return Trie_get(context->opcodeMap, ident->ptr);
}

inline void GContext_addRecord(GContext *context, const Identifier *ident, Record *record) {
  Array_append(context->recordArray, record, 1);
  void *ndx = (void *) (uint64_t) Array_length(context->recordArray);
  Trie_set(context->objectMap, ident->ptr, ndx);
}

inline void *GContext_findRecord(GContext *context, const Identifier *ident) {
  uint32_t ndx = (uint64_t) Trie_get(context->objectMap, ident->ptr);
  if (!ndx) { return nullptr; }
  return Array_real_addr(context->recordArray, ndx - 1);
}

#define contextAddRecord_DEF(type, array, obj)                             \
  inline uint32_t GContext_add##type(GContext *context, const type *obj) { \
    uint32_t offset = Array_length(context->array);                        \
    Record record = {enum_##type, offset};                                 \
    Array_append(context->array, obj, 1);                                  \
    GContext_addRecord(context, obj->name, &record);                       \
    return offset;                                                         \
  }
contextAddRecord_DEF(Immediate, immArray, imm);
contextAddRecord_DEF(Register, regArray, reg);
contextAddRecord_DEF(Memory, memArray, mem);
contextAddRecord_DEF(RegisterGroup, grpArray, grp);
contextAddRecord_DEF(Set, setArray, set);

#define contextGetFromOffset_DEF(type, array)                           \
  inline type *GContext_get##type(GContext *context, uint32_t offset) { \
    return Array_real_addr(context->array, offset);                     \
  }

contextGetFromOffset_DEF(Immediate, immArray);
contextGetFromOffset_DEF(Register, regArray);
contextGetFromOffset_DEF(Memory, memArray);
contextGetFromOffset_DEF(RegisterGroup, grpArray);
contextGetFromOffset_DEF(Set, setArray);

inline void *GContext_findIdentInStack(GContext *context, Identifier *ident) {
  const uint32_t length = Stack_size(context->identStack) / sizeof(Identifier *);
  const Identifier * const * const idents = Stack_get(context->identStack, 0);
  for (uint32_t i = 0; i < length; i++) {
    const Identifier *id = idents[i];
    if (Identifier_cmp(ident, id) == 0) { return (void *) id; }
  }
  return nullptr;
}

inline void GContext_addPattern(GContext *context, Pattern *pattern) {
  if (!context->patterns) {
    context->patterns = Array_new(sizeof(Pattern *), enum_Pattern, context->allocator);
  }
  Array_append(context->patterns, &pattern, 1);
}

bool GContext_testPattern(GContext *context, PatternArgs *patternArgs) {
  if (!context->patterns) { return false; }
  const uint32_t length = Array_length(context->patterns);
  const Pattern * const * const patterns = Array_real_addr(context->patterns, 0);
  for (uint32_t i = 0; i < length; i++) {
    const Pattern * const temp = patterns[i];
    if (PatternArgs_cmp(temp->args, patternArgs) == 0) { return true; }
  }
  return false;
}

uint64_t GContext_getLastWidth(GContext *context) {
  uint64_t width = 0;
  Stack_top(context->widthStack, &width, sizeof(uint64_t));
  return width;
}

inline void GContext_addMapItem(GContext *context, MappingItem *item) {
  AVLTree_set(context->mappingTree, (uint64_t) item->field, item->evaluable);
}

inline MappingItem *GContext_getMapItem(GContext *context, BitField *bf) {
  return AVLTree_get(context->mappingTree, (uint64_t) bf);
}

void push_context_ident(GContext *context, void *token) {
  Stack_push(context->identStack, &token, sizeof(uint64_t));
}

void realloc_context_map_item_tree(GContext *context, void *) {
  context->mappingTree = AVLTree_new(context->allocator, (compare_t *) BitField_cmp);
}

void destroy_context_map_item_tree(GContext *context, void *) {
  if (context->mappingTree) { AVLTree_destroy(context->mappingTree, nullptr); }
  context->mappingTree = nullptr;
}

void pop_context_ident(GContext *context, void *) {
  Stack_pop(context->identStack, nullptr, sizeof(void *));
}

void push_context_width(GContext *context, void *token) {
  uint64_t width = (uint64_t) token;
  Stack_push(context->widthStack, &width, sizeof(uint64_t));
}
void pop_context_width(GContext *context, void *) {
  Stack_pop(context->widthStack, nullptr, sizeof(uint64_t));
}

void pop_context_width_and_ident(GContext *context, void *token) {
  pop_context_width(context, token);
  pop_context_ident(context, token);
}

void destroy_map_item_tree_and_pop_width(GContext *context, void *token) {
  pop_context_width(context, token);
  destroy_context_map_item_tree(context, token);
}

#include "action-table.gen.h"
#define IN_MACHINE(s)     __MACHINE_IDENTIFIER_LEFT_BRACKET_##s
#define IN_REGISTER(s)    __MACHINE_IDENTIFIER_LEFT_BRACKET_REGISTER_IDENTIFIER_WIDTH_LEFT_BRACKET_##s
#define IN_INSTRUCTION(s) __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_##s
#define IN_INSTR_FORM(s) \
  __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_Pattern_EQUAL_WIDTH_LEFT_BRACKET_##s
#define IN_INSTR_PART(s) \
  __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_Pattern_EQUAL_WIDTH_LEFT_BRACKET_PART_KEY_COLON_WIDTH_EQUAL_LEFT_BRACKET_##s

fn_ctx_act *get_after_stack_actions(int32_t state) {
  switch (state) {
    case __MACHINE_IDENTIFIER:
    case IN_MACHINE(REGISTER_IDENTIFIER): {
      return push_context_ident;
    }
    case IN_MACHINE(REGISTER_IDENTIFIER_WIDTH):
    case IN_MACHINE(MEMORY_IDENTIFIER_WIDTH):
    case IN_INSTRUCTION(Pattern_EQUAL_WIDTH):
    case IN_INSTR_FORM(PART_KEY_COLON_WIDTH): {
      return push_context_width;
    }
    case IN_INSTR_FORM(PART_KEY_COLON_WIDTH_EQUAL_LEFT_BRACKET): {
      return realloc_context_map_item_tree;
    }
    case IN_REGISTER(Registers_RIGHT_BRACKET): {
      return pop_context_width_and_ident;
    }
    case IN_INSTR_PART(MappingItems_RIGHT_BRACKET): {
      return destroy_context_map_item_tree;
    }
  }
  return nullptr;
}
fn_ctx_act *get_after_reduce_actions(int32_t state) {
  switch (state) {
    case __Machine: {
      return pop_context_ident;
    }
    case IN_MACHINE(Memory):
    case IN_INSTR_FORM(InstrPart):
    case IN_INSTRUCTION(InstrForm):
    case IN_INSTRUCTION(InstrForms_InstrForm): {
      return pop_context_width;
    }
  }
  return nullptr;
}
