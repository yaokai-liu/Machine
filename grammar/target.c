/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: target.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-03
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include "tokens.gen.h"
#include <stdio.h>

void releaseIdentifier(Identifier *, const Allocator *) {}

void releaseBitField(BitField *, const Allocator *) {}

void releaseEntry(Entry *entry, const Allocator *allocator) {
  fn_free_t *fn_release = nullptr;
  switch (entry->type) {
    case enum_RegisterGroup: {
      fn_release = (fn_free_t *) releaseRegisterGroup;
      break;
    }
    case enum_Instruction: {
      fn_release = (fn_free_t *) releaseInstruction;
      break;
    }
    case enum_Memory: {
      fn_release = (fn_free_t *) releaseMemory;
      break;
    }
    case enum_Immediate: {
      fn_release = (fn_free_t *) releaseImmediate;
      break;
    }
    case enum_Set: {
      fn_release = (fn_free_t *) releaseSet;
      break;
    }
  }
  fn_release(entry->target, allocator);
  allocator->free(entry->target);
}

void releaseMachine(Machine *machine, const Allocator *allocator) {
  releaseIdentifier(machine->name, allocator);
  allocator->free(machine->name);
  Array_reset(machine->entries, (fn_free_t *) releaseEntry);
  Array_destroy(machine->entries);
  // TODO: release other arrays.
}

void releaseImmediate(Immediate *immediate, const Allocator *allocator) {
  releaseIdentifier(immediate->name, allocator);
  allocator->free(immediate->name);
}

void releasePattern(Pattern *pattern, const Allocator *) {
  Array_reset(pattern->args, (fn_free_t *) releaseIdentifier);
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

void releaseLayout(Layout *layout, const Allocator *allocator) {
  switch (layout->type) {
    case enum_Evaluable: {
      releaseEvaluable(layout->target, allocator);
      allocator->free(layout->target);
      return;
    }
    case enum_MappingItems: {
      Array_reset(layout->target, (fn_free_t *) releaseMappingItem);
      Array_destroy(layout->target);
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
  Array_reset(form->parts, (fn_free_t *) releaseInstrPart);
  Array_destroy(form->parts);
}

void releaseInstruction(Instruction *instr, const Allocator *allocator) {
  releaseIdentifier(instr->name, allocator);
  allocator->free(instr->name);
  Array_reset(instr->forms, (fn_free_t *) releaseInstrForm);
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
  Array_reset(rg->registers, (fn_free_t *) releaseRegister);
  Array_destroy(rg->registers);
}

void releaseSetItem(SetItem *item, const Allocator *allocator) {
  releaseIdentifier(item->name, allocator);
  allocator->free(item->name);
}

void releaseSet(Set *set, const Allocator *allocator) {
  releaseIdentifier(set->name, allocator);
  allocator->free(set->name);
  Array_reset(set->items, (fn_free_t *) releaseSetItem);
  Array_destroy(set->items);
}
