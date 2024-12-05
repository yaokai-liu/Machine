/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: context.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "context.h"
#include "trie.h"

typedef struct Namespace {
  Trie *trie;
} Namespace;

typedef struct GContext {
  const Allocator *allocator;
  Entries *entries;
  Namespace *inmOpcode;
  Namespace *inmPattern;
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
  context->inmPattern = Namespace_new(allocator);
  return context;
}

inline void GContext_addOpcode(GContext *context, Identifier *ident, Instruction *instr) {
  Trie_set(context->inmOpcode->trie, ident->ptr, instr);
}

inline void GContext_addPattern(GContext *context, Identifier *ident, Entry *entry) {
  Array_append(context->entries, entry, 1);
  void *e = (void *) (uint64_t) Array_length(context->entries);
  Trie_set(context->inmPattern->trie, ident->ptr, e);
}

inline void *GContext_findOpcode(GContext *context, Identifier *ident) {
  return Trie_get(context->inmOpcode->trie, ident->ptr);
}

inline void *GContext_findPattern(GContext *context, Identifier *ident) {
  uint32_t ndx = (uint64_t) Trie_get(context->inmPattern->trie, ident->ptr);
  if (!ndx) { return nullptr; }
  return Array_get(context->entries, ndx - 1);
}

inline void Namespace_destroy(Namespace *ns) {
  Trie_destroy(ns->trie);
}

inline void GContext_destroy(GContext *context, const Allocator * allocator) {
  Namespace_destroy(context->inmOpcode);
  Namespace_destroy(context->inmPattern);
  releasePrimeArray(context->entries);
  context->allocator->free(context->inmOpcode);
  context->allocator->free(context->inmPattern);
  allocator->free(context);
}
