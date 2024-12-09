/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: target.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-03
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include "avl-tree.h"
#include "context.h"
#include "tokens.gen.h"

void releaseIdentifier(Identifier *ident, const Allocator *allocator) {
  allocator->free(ident->ptr);
}

void releaseBitField(BitField *, const Allocator *) {}

void releaseEntry(Entry *entry, const Allocator *allocator) {
  destruct_t *fn_release = nullptr;
  switch (entry->type) {
    case enum_RegisterGroup: {
      fn_release = (destruct_t *) releaseRegisterGroup;
      break;
    }
    case enum_Instruction: {
      fn_release = (destruct_t *) releaseInstruction;
      break;
    }
    case enum_Memory: {
      fn_release = (destruct_t *) releaseMemory;
      break;
    }
    case enum_Immediate: {
      fn_release = (destruct_t *) releaseImmediate;
      break;
    }
    case enum_Set: {
      fn_release = (destruct_t *) releaseSet;
      break;
    }
  }
  fn_release(entry->target, allocator);
  allocator->free(entry->target);
}

void releaseMachine(Machine *machine, const Allocator *allocator) {
  releaseIdentifier(machine->name, allocator);
  allocator->free(machine->name);
  Array_reset(machine->entries, (destruct_t *) releaseEntry);
  Array_destroy(machine->entries);
  GContext_destroy(machine->context, allocator);
  // TODO: release other arrays.
}

void releaseImmediate(Immediate *immediate, const Allocator *allocator) {
  releaseIdentifier(immediate->name, allocator);
  allocator->free(immediate->name);
}

void releasePattern(Pattern *pattern, const Allocator *) {
  Array_reset(pattern->args, (destruct_t *) releaseIdentifier);
  Array_destroy(pattern->args);
}

void releaseEvaluable(Evaluable *evaluable, const Allocator *allocator) {
  if (enum_NUMBER == evaluable->type) { return; }
  if (enum_BIT_FIELD == evaluable->type) {
    releaseBitField(evaluable->rhs, allocator);
    allocator->free(evaluable->rhs);
  }
  releaseIdentifier(evaluable->lhs, allocator);
  allocator->free(evaluable->lhs);
  return;
}

void releaseMappingItem(MappingItem *item, const Allocator *allocator) {
  releaseBitField(item->field, allocator);
  allocator->free(item->field);
  releaseEvaluable(item->evaluable, allocator);
  allocator->free(item->evaluable);
}

void releaseMappingItems(MappingItems *items, const Allocator *) {
  if (items->itemTree) { AVLTree_destroy(items->itemTree, nullptr); }
  Array_reset(items->itemArray, (destruct_t *) releaseMappingItem);
  Array_destroy(items->itemArray);
}

void releaseLayout(Layout *layout, const Allocator *allocator) {
  switch (layout->type) {
    case enum_Evaluable: {
      releaseEvaluable(layout->target, allocator);
      allocator->free(layout->target);
      return;
    }
    case enum_MappingItems: {
      releaseMappingItems(layout->target, allocator);
      allocator->free(layout->target);
    }
  }
}

void releaseInstrPart(InstrPart *part, const Allocator *allocator) {
  releaseLayout(part->layout, allocator);
  allocator->free(part->layout);
}

void releaseInstrForm(InstrForm *form, const Allocator *allocator) {
  releasePattern(form->pattern, allocator);
  allocator->free(form->pattern);
  for (int i = 0; i < 3; i++) {
    Layout *layout = form->parts[i].layout;
    if (layout) { releaseLayout(layout, allocator); }
    allocator->free(layout);
  }
}

void releaseInstruction(Instruction *instr, const Allocator *allocator) {
  releaseIdentifier(instr->name, allocator);
  allocator->free(instr->name);
  Array_reset(instr->forms, (destruct_t *) releaseInstrForm);
  Array_destroy(instr->forms);
}

void releaseMemItem(MemItem *item, const Allocator *allocator) {
  releaseBitField(item->field, allocator);
  allocator->free(item->field);
}

void releaseMemory(Memory *memory, const Allocator *allocator) {
  releaseIdentifier(memory->name, allocator);
  releaseBitField(memory->base, allocator);
  releaseBitField(memory->offset, allocator);
  allocator->free(memory->name);
  allocator->free(memory->base);
  allocator->free(memory->offset);
}

void releaseRegister(Register *reg, const Allocator *allocator) {
  releaseIdentifier(reg->name, allocator);
  releaseBitField(reg->field, allocator);
  allocator->free(reg->name);
  allocator->free(reg->field);
}

void releaseRegisterGroup(RegisterGroup *rg, const Allocator *allocator) {
  releaseIdentifier(rg->name, allocator);
  allocator->free(rg->name);
  Array_reset(rg->registers, (destruct_t *) releaseRegister);
  Array_destroy(rg->registers);
}

void releaseSetItem(SetItem *item, const Allocator *allocator) {
  releaseIdentifier(item->name, allocator);
  allocator->free(item->name);
}

void releaseSet(Set *set, const Allocator *allocator) {
  releaseIdentifier(set->name, allocator);
  allocator->free(set->name);
  Array_reset(set->items, (destruct_t *) releaseSetItem);
  Array_destroy(set->items);
}

#include "string_t.h"

inline int32_t Identifier_cmp(const Identifier *ident1, const Identifier *ident2) {
  if (!ident1) { return 1; }
  if (!ident2) { return -1; }
  if (ident1->len < ident2->len) { return -1; }
  if (ident1->len > ident2->len) { return 1; }
  uint32_t cmp_len = strcmp_o(ident1->ptr, ident2->ptr);
  return (int32_t) (ident1->len < cmp_len) ? -1 : (ident1->len > cmp_len) ? 1 : 0;
}

inline int32_t PatternArgs_cmp(PatternArgs *args1, PatternArgs *args2) {
  if (!args1) { return 1; }
  if (!args2) { return -1; }
  const uint32_t len1 = Array_length(args1);
  const uint32_t len2 = Array_length(args2);
  if (len1 < len2) { return -1; }
  if (len1 > len2) { return 1; }

  const Identifier * const idents1 = Array_get(args1, 0);
  const Identifier * const idents2 = Array_get(args2, 0);
  for (uint32_t i = 0; i < len1; i++) {
    const Identifier * const ident1 = &idents1[i];
    const Identifier * const ident2 = &idents2[i];
    int32_t b = Identifier_cmp(ident1, ident2);
    if (b != 0) { return b; }
  }
  return 0;
}

inline int32_t BitField_cmp(BitField *bf1, BitField *bf2) {
  if (!bf1) { return -1; }
  if (!bf2) { return 1; }
  if (bf1->upper < bf2->lower) { return -1; }
  if (bf1->lower > bf2->upper) { return 1; }
  return 0;
}
