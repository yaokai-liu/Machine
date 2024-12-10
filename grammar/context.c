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
#include "trie.h"
#include <stdint.h>

typedef struct Namespace {
  Trie *trie;
} Namespace;

typedef struct GContext {
  const Allocator *allocator;
  Entries *entries;
  codegen_t *(*getCodegen)(uint32_t token_type);
  Namespace *inmOpcode;
  Namespace *inmRefer;
  Array *patterns;
  Stack *widthStack;
  Stack *identStack;
  AVLTree *mapTree;
} GContext;

Namespace *Namespace_new(const Allocator *allocator);
void Namespace_destroy(Namespace *ns);

inline Namespace *Namespace_new(const Allocator *allocator) {
  Namespace *ns = allocator->calloc(1, sizeof(Namespace));
  ns->trie = Trie_new(allocator);
  return ns;
}

inline GContext *GContext_new(const Allocator *allocator) {
  GContext *context = allocator->calloc(1, sizeof(GContext));
  context->allocator = allocator;
  context->entries = Array_new(sizeof(Entry), allocator);
  context->inmOpcode = Namespace_new(allocator);
  context->inmRefer = Namespace_new(allocator);
  context->widthStack = Stack_new(allocator);
  context->identStack = Stack_new(allocator);
  context->mapTree = nullptr;
  return context;
}

inline const Allocator *GContext_getAllocator(GContext *context) {
  return context->allocator;
}

inline void GContext_setCodegen(GContext *context, codegen_t *(*getCodegen)(uint32_t token_type)) {
  context->getCodegen = getCodegen;
}

inline codegen_t *GContext_getCodegen(GContext *context, uint32_t token_type) {
  if (context->getCodegen) { return context->getCodegen(token_type); }
  return nullptr;
}

inline void GContext_addOpcode(GContext *context, Identifier *ident, Instruction *instr) {
  Trie_set(context->inmOpcode->trie, ident->ptr, instr);
}
inline void *GContext_findOpcode(GContext *context, Identifier *ident) {
  return Trie_get(context->inmOpcode->trie, ident->ptr);
}

inline void GContext_addRefer(GContext *context, Identifier *ident, Entry *entry) {
  Array_append(context->entries, entry, 1);
  void *e = (void *) (uint64_t) Array_length(context->entries);
  Trie_set(context->inmRefer->trie, ident->ptr, e);
}

inline void *GContext_findRefer(GContext *context, Identifier *ident) {
  uint32_t ndx = (uint64_t) Trie_get(context->inmRefer->trie, ident->ptr);
  if (!ndx) { return nullptr; }
  return Array_get(context->entries, ndx - 1);
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

inline void GContext_addPattern(GContext *context, Pattern *pattern) {
  if (!context->patterns) { context->patterns = Array_new(sizeof(Pattern *), context->allocator); }
  Array_append(context->patterns, &pattern, 1);
}

bool GContext_testPattern(GContext *context, PatternArgs *patternArgs) {
  if (!context->patterns) { return false; }
  const uint32_t length = Array_length(context->patterns);
  const Pattern * const * const patterns = Array_get(context->patterns, 0);
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

inline void Namespace_destroy(Namespace *ns) {
  Trie_destroy(ns->trie);
}

#define contextReleaseStack(stack)            \
  do {                                        \
    Stack_clear(context->stack);              \
    context->allocator->free(context->stack); \
  } while (false)

inline void GContext_destroy(GContext *context, const Allocator *allocator) {
  Namespace_destroy(context->inmOpcode);
  Namespace_destroy(context->inmRefer);
  releasePrimeArray(context->entries);
  context->allocator->free(context->inmOpcode);
  context->allocator->free(context->inmRefer);
  contextReleaseStack(widthStack);
  contextReleaseStack(identStack);
  if (context->patterns) { releasePrimeArray(context->patterns); }
  if (context->mapTree) { AVLTree_destroy(context->mapTree, nullptr); }
  allocator->free(context);
}

inline void GContext_addMapItem(GContext *context, MappingItem *item) {
  AVLTree_set(context->mapTree, (uint64_t) item->field, item->evaluable);
}

inline MappingItem *GContext_getMapItem(GContext *context, BitField *bf) {
  return AVLTree_get(context->mapTree, (uint64_t) bf);
}

void push_context_ident(GContext *context, void *token) {
  Stack_push(context->identStack, &token, sizeof(uint64_t));
}

void realloc_context_map_item_tree(GContext *context, void *) {
  context->mapTree = AVLTree_new(context->allocator, (compare_t *) BitField_cmp);
}

void destroy_context_map_item_tree(GContext *context, void *) {
  if (context->mapTree) { AVLTree_destroy(context->mapTree, nullptr); }
  context->mapTree = nullptr;
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
