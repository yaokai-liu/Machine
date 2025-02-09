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
#include "trie-dump.h"
#include "trie.h"
#include <stdint.h>

#define max(a, b) ((a) > (b) ? (a) : (b))

uint64_t getchar(const void *key) {
  return *(const char_t *) key;
}
uint64_t get_record_ndx(const void *ndx_ptr) {
  return *(const uint64_t *) ndx_ptr;
}

inline GContext *GContext_new(const Allocator *allocator) {
  GContext *context = allocator->calloc(1, sizeof(GContext));
  context->allocator = allocator;
  context->regArray = Array_new(sizeof(Register), enum_Register, allocator);
  context->immArray = Array_new(sizeof(Immediate), enum_Immediate, allocator);
  context->memArray = Array_new(sizeof(Memory), enum_Memory, allocator);
  context->setArray = Array_new(sizeof(Set), enum_Set, allocator);
  context->grpArray = Array_new(sizeof(RegisterGroup), enum_RegisterGroup, allocator);
  context->instrArray = Array_new(sizeof(Instruction), enum_Instruction, allocator);
  context->recordArray = Array_new(sizeof(Record), INT32_MAX - 1, allocator);
  context->keyArray = Array_new(sizeof(TrieKeyItem), INT32_MAX - 2, allocator);
  context->stateArray = Array_new(sizeof(TrieNodeItem), INT32_MAX - 3, allocator);
  context->objectMap = Trie_new(sizeof(char_t), getchar, allocator);
  context->opcodeMap = Trie_new(sizeof(char_t), getchar, allocator);
  context->widthStack = Stack_new(allocator);
  context->identStack = Stack_new(allocator);
  context->mappingTree = nullptr;
  context->errorMessage = nullptr;
  context->maxArgCount = 0x10;
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
  contextReleaseArray(instrArray, releaseInstruction);
  releasePrimeArray(context->keyArray);
  releasePrimeArray(context->stateArray);
  releasePrimeArray(context->recordArray);
  Trie_destroy(context->objectMap);
  Trie_destroy(context->opcodeMap);
  contextReleaseStack(widthStack);
  contextReleaseStack(identStack);
  context->allocator->free(context);
}

inline const Allocator *GContext_getAllocator(const GContext *context) {
  return context->allocator;
}

inline void GContext_addOpcode(GContext *context, const Identifier *ident, Instruction *instr) {
  Trie_set(context->opcodeMap, ident->ptr, instr);
}
inline Instruction *GContext_findOpcode(GContext *context, const Identifier *ident) {
  return Trie_get(context->opcodeMap, ident->ptr);
}

inline void GContext_addRecord(GContext *context, const Identifier *ident, Record *record) {
  Array_append(context->recordArray, record, 1);
  void *ndx = (void *) (uint64_t) Array_length(context->recordArray);
  Trie_set(context->objectMap, ident->ptr, ndx);
}

inline const Record *GContext_findRecord(const GContext *context, const Identifier *ident) {
  uint32_t ndx = (uint64_t) Trie_get(context->objectMap, ident->ptr);
  if (!ndx) { return nullptr; }
  return Array_real_addr(context->recordArray, ndx - 1);
}

#define contextAddRecord_DEF(type, array, obj)                                \
  inline REFER(type) GContext_add##type(GContext *context, const type *obj) { \
    uint32_t offset = Array_length(context->array);                           \
    Record record = {enum_##type, offset};                                    \
    Array_append(context->array, obj, 1);                                     \
    GContext_addRecord(context, obj->name, &record);                          \
    return Array_virt_addr(context->array, offset);                           \
  }
contextAddRecord_DEF(Immediate, immArray, imm);
contextAddRecord_DEF(Register, regArray, reg);
contextAddRecord_DEF(Memory, memArray, mem);
contextAddRecord_DEF(RegisterGroup, grpArray, grp);
contextAddRecord_DEF(Set, setArray, set);

inline REFER(Instruction) GContext_addInstruction(GContext *context, const Instruction *instr) {
  uint32_t ndx = Array_length(context->instrArray);
  Array_append(context->instrArray, instr, 1);
  REFER(Instruction) v_instr = Array_virt_addr(context->instrArray, ndx);
  GContext_addOpcode(context, instr->name, v_instr);
  return v_instr;
}

#define contextGetFromOffset_DEF(type, array)                                       \
  inline const type *GContext_get##type(const GContext *context, uint32_t offset) { \
    return Array_real_addr(context->array, offset);                                 \
  }

contextGetFromOffset_DEF(Immediate, immArray);
contextGetFromOffset_DEF(Register, regArray);
contextGetFromOffset_DEF(Memory, memArray);
contextGetFromOffset_DEF(RegisterGroup, grpArray);
contextGetFromOffset_DEF(Set, setArray);

inline const Instruction *GContext_getInstruction(const GContext *context, uint32_t index) {
  return Array_real_addr(context->instrArray, index);
}

inline void *GContext_findIdentInStack(GContext *context, Identifier *ident) {
  const uint32_t length = Stack_size(context->identStack) / sizeof(Identifier *);
  const Identifier * const * const idents = Stack_get(context->identStack, 0);
  for (uint32_t i = 0; i < length; i++) {
    const Identifier *id = idents[i];
    if (Identifier_cmp(ident, id) == 0) { return (void *) id; }
  }
  return nullptr;
}

void Gcontext_setParts(GContext *context, InstrParts *parts) {
  context->parts = parts;
}

void Gcontext_setItems(GContext *context, MemItems *items) {
  context->items = items;
}

const InstrPart *GContext_findInstrPart(GContext *context, Identifier *ident) {
  if (!context->parts) { return nullptr; }
  const uint32_t length = Array_length(context->parts);
  const InstrPart * const parts = Array_real_addr(context->parts, 0);
  for (uint32_t i = 0; i < length; i++) {
    if (Identifier_cmp(parts[i].name, ident) == 0) { return Array_virt_addr(context->parts, i); }
  }
  return nullptr;
}

const MemItem *GContext_findMemItem(GContext *context, Identifier *ident) {
  if (!context->items) { return nullptr; }
  const uint32_t length = Array_length(context->items);
  const MemItem * const items = Array_real_addr(context->items, 0);
  for (uint32_t i = 0; i < length; i++) {
    if (Identifier_cmp(items[i].name, ident) == 0) { return Array_virt_addr(context->items, i); }
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
  AVLTree_set(context->mappingTree, (uint64_t) item->field, item);
}

inline MappingItem *GContext_getMapItem(GContext *context, BitField *bf) {
  return AVLTree_get(context->mappingTree, (uint64_t) bf);
}

inline const Parameter *GContext_findParameter(GContext *context, Identifier *ident) {
  if (!context->patterns) { return nullptr; }
  uint32_t n_patterns = Array_length(context->patterns);
  if (n_patterns == 0) { return nullptr; }
  Pattern *pattern = *(Pattern **) Array_real_addr(context->patterns, n_patterns - 1);
  const uint32_t n_args = Array_length(pattern->args);
  const Parameter *args = Array_real_addr(pattern->args, 0);
  for (uint32_t i = 0; i < n_args; i++) {
    if (Identifier_cmp(ident, args[i].name) == 0) { return &args[i]; }
  }
  return nullptr;
}

#define instrFormNdx(instr, i) ((void *) (((uint64_t) (instr_ndx)) << 32) + ((i) + 1))
Trie /*<REFER(Record), uint64_t>*/ *
    GContext_build_args_trie(GContext *context, const Instruction *instr) {
  Trie /*<REFER(Record), uint64_t>*/ *args_trie =
      Trie_new(sizeof(void *), get_record_ndx, context->allocator);
  const uint32_t n_forms = Array_length(instr->forms);
  const InstrForm *forms = Array_real_addr(instr->forms, 0);
  REFER(Record) *ndx_array = nullptr;
  const uint32_t instr_ndx = (uint32_t) (uint64_t) GContext_findOpcode(context, instr->name);
  for (uint64_t i = 0; i < n_forms; i++) {
    Pattern *pattern = forms[i].pattern;
    if (!pattern->args) {
      const uint64_t ndx = 0LLU;
      Trie_set(args_trie, &ndx, instrFormNdx(instr_ndx, i));
    } else {
      const uint32_t length = Array_length(pattern->args);
      void *p = context->allocator->realloc(ndx_array, (length + 1) * sizeof(REFER(Record)));
      if (p) {
        ndx_array = p;
      } else {
        if (ndx_array) { context->allocator->free(ndx_array); }
        Trie_destroy(args_trie);
        return nullptr;
      }
      const Parameter *idents = Array_real_addr(pattern->args, 0);
      for (uint32_t j = 0; j < length; j++) {
        uint64_t ndx = (uint64_t) Trie_get(context->objectMap, idents[j].type->ptr);
        ndx_array[j] = Array_virt_addr(context->recordArray, ndx - 1);
      }
      ndx_array[length] = nullptr;
      Trie_set(args_trie, ndx_array, instrFormNdx(instr_ndx, i));
    }
  }
  if (ndx_array) { context->allocator->free(ndx_array); }
  return args_trie;
}

void GContext_dump_instruction(GContext *context, Instruction *instr) {
  instr = Array_vert2real(context->instrArray, instr);
  Trie /*<REFER(Record), uint64_t>*/ *args_trie = GContext_build_args_trie(context, instr);
  if (!args_trie) { return; }
  Trie_dump(args_trie, context->keyArray, context->stateArray);
  instr->entry_offset = Array_length(context->stateArray);
  Trie_destroy(args_trie);
}

inline void GContext_setErrorMessage(GContext *context, const char_t *msg) {
  context->errorMessage = msg;
}
const char_t *GContext_getErrorMessage(GContext *context) {
  return context->errorMessage;
}

void realloc_context_map_item_tree(GContext *context, void *) {
  context->mappingTree = AVLTree_new(context->allocator, (compare_t *) BitField_cmp);
}

void destroy_context_map_item_tree(GContext *context, void *) {
  if (context->mappingTree) { AVLTree_destroy(context->mappingTree, nullptr); }
  context->mappingTree = nullptr;
}

void push_context_ident(GContext *context, void *token) {
  Identifier *ident = (Identifier *) token;
  Stack_push(context->identStack, &ident, sizeof(Identifier *));
}

void pop_context_ident(GContext *context, void *) {
  Stack_pop(context->identStack, nullptr, sizeof(Identifier *));
}

void push_context_width(GContext *context, void *token) {
  uint64_t width = (uint64_t) token;
  Stack_push(context->widthStack, &width, sizeof(uint64_t));
}
void pop_context_width(GContext *context, void *) {
  Stack_pop(context->widthStack, nullptr, sizeof(uint64_t));
}

void pop_context_width_and_ident(GContext *context, void *) {
  pop_context_width(context, nullptr);
  pop_context_ident(context, nullptr);
}
void pop_context_width_and_set_parts_null(GContext *context, void *) {
  pop_context_width(context, nullptr);
  Gcontext_setParts(context, nullptr);
}
void pop_context_width_and_set_items_null(GContext *context, void *) {
  pop_context_width(context, nullptr);
  Gcontext_setItems(context, nullptr);
}

void release_ctx_patterns(GContext *context, void *) {
  releasePrimeArray(context->patterns);
  context->patterns = nullptr;
}

void destroy_map_item_tree_and_pop_width(GContext *context, void *) {
  pop_context_width(context, nullptr);
  destroy_context_map_item_tree(context, nullptr);
}

#include "action-table.gen.h"
#define IN_MACHINE(s)     __MACHINE_IDENTIFIER_LEFT_BRACKET_##s
#define IN_REGISTER(s)    __MACHINE_IDENTIFIER_LEFT_BRACKET_REGISTER_IDENTIFIER_WIDTH_LEFT_BRACKET_##s
#define IN_INSTRUCTION(s) __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_##s
#define IN_INSTR_FORM(s) \
  __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_Pattern_EQUAL_WIDTH_LEFT_BRACKET_##s
#define IN_INSTR_PART(s) \
  __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_Pattern_EQUAL_WIDTH_LEFT_BRACKET_IDENTIFIER_COLON_WIDTH_EQUAL_LEFT_BRACKET_##s

fn_ctx_act *get_after_stack_actions(int32_t state) {
  switch (state) {
    case __MACHINE_IDENTIFIER:
    case IN_MACHINE(REGISTER_IDENTIFIER): {
      return push_context_ident;
    }
    case IN_MACHINE(REGISTER_IDENTIFIER_WIDTH):
    case IN_MACHINE(MEMORY_IDENTIFIER_WIDTH):
    case IN_INSTRUCTION(Pattern_EQUAL_WIDTH):
    case IN_INSTR_FORM(IDENTIFIER_COLON_WIDTH): {
      return push_context_width;
    }
    case IN_INSTR_FORM(IDENTIFIER_COLON_WIDTH_EQUAL_LEFT_BRACKET): {
      return realloc_context_map_item_tree;
    }
    case IN_REGISTER(Registers_RIGHT_BRACKET): {
      return pop_context_width_and_ident;
    }
    case IN_INSTR_PART(MappingItems_RIGHT_BRACKET): {
      return destroy_context_map_item_tree;
    }
    default: {
      return nullptr;
    }
  }
}
fn_ctx_act *get_after_reduce_actions(int32_t state) {
  switch (state) {
    case __Machine: {
      return pop_context_ident;
    }
    case IN_MACHINE(Memory): {
      return pop_context_width_and_set_items_null;
    }
    case IN_INSTR_FORM(InstrPart): {
      return pop_context_width;
    }
    case IN_INSTRUCTION(InstrForms_InstrForm):
    case IN_INSTRUCTION(InstrForm): {
      return pop_context_width_and_set_parts_null;
    }
    case IN_MACHINE(Instruction): {
      return release_ctx_patterns;
    }
    default: {
      return nullptr;
    }
  }
}
