/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: context.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "context.h"
#include "stack.h"
#include "trie.h"

typedef struct Namespace {
  Trie *trie;
} Namespace;

typedef struct GContext {
  const Allocator *allocator;
  Entries *entries;
  Namespace *inmOpcode;
  Namespace *inmRefer;
  Array *patterns;
  Stack *width_stack;
  Stack *ident_stack;
} GContext;

Namespace *Namespace_new(const Allocator *allocator);
void Namespace_destroy(Namespace *ns);

inline Namespace *Namespace_new(const Allocator *allocator) {
  Namespace *ns = allocator->calloc(1, sizeof(allocator));
  ns->trie = Trie_new(allocator);
  return ns;
}

inline GContext *GContext_new(const Allocator *allocator) {
  GContext *context = allocator->calloc(1, sizeof(GContext));
  context->allocator = allocator;
  context->entries = Array_new(sizeof(Entry), allocator);
  context->inmOpcode = Namespace_new(allocator);
  context->inmRefer = Namespace_new(allocator);
  context->width_stack = Stack_new(allocator);
  context->ident_stack = Stack_new(allocator);
  return context;
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
  const uint32_t length = Stack_size(context->ident_stack) / sizeof(Identifier *);
  const Identifier * const * const idents = Stack_get(context->ident_stack, 0);
  for (uint32_t i = 0; i < length; i++) {
    const Identifier *id = idents[i];
    if (Identifier_cmp(ident, id)) { return (void *) id; }
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
    if (PatternArgs_cmp(temp->args, patternArgs)) { return true; }
  }
  return false;
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
  contextReleaseStack(width_stack);
  contextReleaseStack(ident_stack);
  if (context->patterns) { releasePrimeArray(context->patterns); }
  allocator->free(context);
}

void push_context_ident(GContext *context, void *token) {
  Stack_push(context->ident_stack, &token, sizeof(uint64_t));
}
void pop_context_ident(GContext *context, void *) {
  Stack_pop(context->ident_stack, nullptr, sizeof(void *));
}

void push_context_width(GContext *context, void *token) {
  uint64_t width = (uint64_t) token;
  Stack_push(context->width_stack, &width, sizeof(uint64_t));
}
void pop_context_width(GContext *context, void *) {
  Stack_pop(context->width_stack, nullptr, sizeof(uint64_t));
}

void pop_context_width_and_ident(GContext *context, void *token) {
  pop_context_width(context, token);
  pop_context_ident(context, token);
}

#include "action-table.gen.h"
#define IN_MACHINE(s)     __MACHINE_IDENTIFIER_LEFT_BRACKET_##s
#define IN_REGISTER(s)    __MACHINE_IDENTIFIER_LEFT_BRACKET_REGISTER_IDENTIFIER_WIDTH_LEFT_BRACKET_##s
#define IN_INSTRUCTION(s) __MACHINE_IDENTIFIER_LEFT_BRACKET_INSTRUCTION_IDENTIFIER_LEFT_BRACKET_##s

fn_ctx_act *get_after_stack_actions(int32_t state) {
  switch (state) {
    case __MACHINE_IDENTIFIER:
    case IN_MACHINE(REGISTER_IDENTIFIER): {
      return push_context_ident;
    }
    case IN_MACHINE(REGISTER_IDENTIFIER_WIDTH):
    case IN_MACHINE(MEMORY_IDENTIFIER_WIDTH):
    case IN_INSTRUCTION(Pattern_EQUAL_WIDTH): {
      return push_context_width;
    }
    case IN_REGISTER(Registers_RIGHT_BRACKET): {
      return pop_context_width_and_ident;
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
    case IN_INSTRUCTION(InstrForm):
    case IN_INSTRUCTION(InstrForms_InstrForm): {
      return pop_context_width;
    }
  }
  return nullptr;
}
