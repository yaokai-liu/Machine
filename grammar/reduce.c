/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: reduce.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "action-table.h"
#include "enum.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"

Entries *p_Entries_0(void *argv[], const Allocator *allocator) {
  Entries *entries = (Entries *) argv[0];
  Entry *entry = (Entry *) argv[1];
  Array_append(entries, entry, 1);
  allocator->free(entry);
  return entries;
}

Entries *p_Entries_1(void *argv[], const Allocator *allocator) {
  Entry *entry = (Entry *) argv[0];
  Entries *entries = Array_new(sizeof(Entry), allocator);
  Array_append(entries, entry, 1);
  allocator->free(entry);
  return entries;
}

Entry *p_Entry_0(void *argv[], const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_RegisterGroup;
  entry->target = (RegisterGroup *) argv[0];
  return entry;
}

Entry *p_Entry_1(void *argv[], const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Instruction;
  entry->target = (Instruction *) argv[0];
  return entry;
}

Entry *p_Entry_2(void *argv[], const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Memory;
  entry->target = (Memory *) argv[0];
  return entry;
}

Entry *p_Entry_3(void *argv[], const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Immediate;
  entry->target = (Immediate *) argv[0];
  return entry;
}

Entry *p_Entry_4(void *argv[], const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Set;
  entry->target = (Set *) argv[0];
  return entry;
}

Evaluable *p_Evaluable_0(void *argv[], const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0];
  // todo: verify identifier in global identifier table.
  void *rhs = argv[2];
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_MEM_KEY;
  evaluable->lhs = lhs;
  evaluable->rhs = rhs;
  return evaluable;
}

Evaluable *p_Evaluable_1(void *argv[], const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0];
  // todo: verify identifier in global identifier table.
  BitField *rhs = (BitField *) argv[1];
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_BIT_FIELD;
  evaluable->lhs = lhs;
  evaluable->rhs = rhs;
  return evaluable;
}

Evaluable *p_Evaluable_2(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[0];
  // todo: verify identifier in global identifier table.
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_IDENTIFIER;
  evaluable->lhs = identifier;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *p_Evaluable_3(void *argv[], const Allocator *allocator) {
  uint64_t number = (uint64_t) argv[0];
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) number;
  evaluable->rhs = nullptr;
  return evaluable;
}

Immediate *p_Immediate_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  // todo: verify identifier not in global identifier table.
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  uint32_t type = (uint64_t) argv[3];
  Immediate *immediate = allocator->calloc(1, sizeof(Immediate));
  immediate->type = type;
  immediate->width = width;
  immediate->name = identifier;
  return immediate;
}

InstrForm *p_InstrForm_0(void *argv[], const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  InstrParts *parts = (InstrParts *) argv[4];
  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = 1;
  form->pattern = pattern;
  form->parts = parts;
  return form;
}

InstrForm *p_InstrForm_1(void *argv[], const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  uint32_t tick = (uint32_t) (uint64_t) argv[3];
  InstrParts *parts = (InstrParts *) argv[5];
  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = tick;
  form->pattern = pattern;
  form->parts = parts;
  return form;
}

InstrForms *p_InstrForms_0(void *argv[], const Allocator *allocator) {
  InstrForms *forms = (InstrForms *) argv[0];
  InstrForm *form = (InstrForm *) argv[1];
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrForms *p_InstrForms_1(void *argv[], const Allocator *allocator) {
  InstrForm *form = (InstrForm *) argv[0];
  InstrForms *forms = Array_new(sizeof(InstrForm), allocator);
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrPart *p_InstrPart_0(void *argv[], const Allocator *allocator) {
  enum PART_KEY key = (uint32_t) (uint64_t) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Layout *layout = (Layout *) argv[4];
  InstrPart *part = allocator->calloc(1, sizeof(InstrPart));
  part->type = key;
  part->width = width;
  part->layout = layout;
  return part;
}

InstrParts *p_InstrParts_0(void *argv[], const Allocator *allocator) {
  InstrParts *parts = (InstrParts *) argv[0];
  InstrPart *part = (InstrPart *) argv[1];
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

InstrParts *p_InstrParts_1(void *argv[], const Allocator *allocator) {
  InstrPart *part = (InstrPart *) argv[0];
  InstrParts *parts = Array_new(sizeof(InstrPart), allocator);
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

Instruction *p_Instruction_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  InstrForms *forms = (InstrForms *) argv[3];
  Instruction *instr = allocator->calloc(1, sizeof(Instruction));
  instr->name = identifier;
  instr->forms = forms;
  return instr;
}

Layout *p_Layout_0(void *argv[], const Allocator *allocator) {
  Evaluable *evaluable = (Evaluable *) argv[0];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Evaluable;
  layout->target = evaluable;
  return layout;
}

Layout *p_Layout_1(void *argv[], const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[1];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_MappingItems;
  layout->target = items;
  return layout;
}

Machine *p_Machine_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  // todo: verify identifier not in global identifier table.
  Entries *entries = argv[3];
  Machine *machine = allocator->calloc(1, sizeof(Machine));
  machine->name = identifier;
  machine->entries = entries;
  return machine;
}

Machine *p___EXTEND_RULE__(void *argv[], const Allocator *) {
  return (Machine *) argv[0];
}

MappingItem *p_MappingItem_0(void *argv[], const Allocator *allocator) {
  BitField *bit_field = (BitField *) argv[0];
  Evaluable *evaluable = (Evaluable *) argv[2];
  MappingItem *item = allocator->calloc(1, sizeof(MappingItem));
  item->field = bit_field;
  item->evaluable = evaluable;
  return item;
}

MappingItems *p_MappingItems_0(void *argv[], const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[0];
  MappingItem *item = (MappingItem *) argv[2];
  Array_append(items, item, 1);
  allocator->free(item);
  return items;
}

MappingItems *p_MappingItems_1(void *argv[], const Allocator *allocator) {
  MappingItem *item = (MappingItem *) argv[0];
  MappingItems *items = Array_new(sizeof(MappingItem), allocator);
  Array_append(items, item, 1);
  allocator->free(item);
  return items;
}

MemItem *p_MemItem_0(void *argv[], const Allocator *allocator) {
  enum MEM_KEY key = (uint32_t) (uint64_t) argv[0];
  if (key != MEM_BASE && key != MEM_OFFSET) { return nullptr; }
  BitField *bit_field = (BitField *) argv[2];
  MemItem *item = allocator->calloc(1, sizeof(MemItem));
  item->type = key;
  item->field = bit_field;
  return item;
}

Memory *p_Memory_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  // todo: verify identifier not in global identifier table.
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  MemItem *item1 = (MemItem *) argv[4];
  MemItem *item2 = (MemItem *) argv[5];
  if (item1->type == item2->type) { return nullptr; }
  Memory *memory = allocator->calloc(1, sizeof(Memory));
  memory->name = identifier;
  memory->width = width;
  if (item1->type == MEM_BASE) {
    memory->base = item1->field;
    memory->offset = item2->field;
  } else {
    memory->base = item2->field;
    memory->offset = item1->field;
  }
  allocator->free(item1);
  allocator->free(item2);
  return memory;
}

Pattern *p_Pattern_0(void *argv[], const Allocator *allocator) {
  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = (PatternArgs *) argv[1];
  return pattern;
}

PatternArgs *p_PatternArgs_0(void *argv[], const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[0];
  Identifier *identifier = (Identifier *) argv[2];
  // todo: verify identifier in global identifier table.
  Array_append(args, identifier, 1);
  allocator->free(identifier);
  return args;
}

PatternArgs *p_PatternArgs_1(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[0];
  // todo: verify identifier in global identifier table.
  PatternArgs *args = Array_new(sizeof(Identifier), allocator);
  Array_append(args, identifier, 1);
  allocator->free(identifier);
  return args;
}

Register *p_Register_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[0];
  // todo: verify identifier not in global identifier table.
  BitField *bit_field = (BitField *) argv[2];
  uint64_t number = (uint64_t) argv[4];
  Register *reg = allocator->calloc(1, sizeof(Register));
  reg->name = identifier;
  reg->field = bit_field;
  reg->code = number;
  return reg;
}

RegisterGroup *p_RegisterGroup_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  // todo: verify identifier not in global identifier table.
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Registers *registers = (Registers *) argv[4];
  RegisterGroup *group = allocator->calloc(1, sizeof(RegisterGroup));
  group->name = identifier;
  group->width = width;
  group->registers = registers;
  return group;
}

Registers *p_Registers_0(void *argv[], const Allocator *allocator) {
  Registers *regs = (Registers *) argv[0];
  Register *reg = (Register *) argv[1];
  Array_append(regs, reg, 1);
  allocator->free(reg);
  return regs;
}

Registers *p_Registers_1(void *argv[], const Allocator *allocator) {
  Register *reg = (Register *) argv[0];
  Registers *regs = Array_new(sizeof(Register), allocator);
  Array_append(regs, reg, 1);
  allocator->free(reg);
  return regs;
}

Set *p_Set_0(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[0];
  // todo: verify identifier not in global identifier table.
  SetItems *items = (SetItems *) argv[1];
  Set *set = allocator->calloc(1, sizeof(Set));
  set->name = identifier;
  set->items = items;
  return set;
}

SetItems *p_SetItems_0(void *argv[], const Allocator *allocator) {
  SetItems *items = (SetItems *) argv[0];
  Identifier *identifier = (Identifier *) argv[2];
  // todo: verify identifier in global identifier table.
  Array_append(items, identifier, 1);
  allocator->free(identifier);
  return items;
}

SetItems *p_SetItems_1(void *argv[], const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[0];
  // todo: verify identifier in global identifier table.
  SetItems *items = Array_new(sizeof(Identifier), allocator);
  Array_append(items, identifier, 1);
  allocator->free(identifier);
  return items;
}

#include "stack.h"

void releaseToken(void *token, uint32_t type, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *result, uint32_t type, const Allocator *allocator
);

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *produceArgs[], int32_t *states, uint32_t argc,
    const Allocator *allocator
);

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *result, uint32_t type, const Allocator *allocator
) {
  int32_t state = 0;
  Stack_top(state_stack, (&state), sizeof(int32_t));
  releaseToken(result, type, allocator);
  allocator->free(result);
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *produceArgs[], int32_t *states, uint32_t argc,
    const Allocator *allocator
) {
  for (uint32_t i = 0; i < argc; i++) {
    uint32_t type = stateCurrentTokenType(states[i]);
    releaseToken(produceArgs[i], type, allocator);
  }
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator) {
  // TODO：release memory allocated.
  void *token = nullptr;
  int32_t state = -1;
  while (!Stack_empty(token_stack)) {
    Stack_pop(token_stack, &token, sizeof(void *));
    Stack_pop(state_stack, &state, sizeof(int32_t));
    uint32_t type = stateCurrentTokenType(state);
    releaseToken(token, type, allocator);
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  return nullptr;
}

#define releaseArrayCase(array, ele)                \
  case enum_##array: {                              \
    Array_reset(token, (fn_free_t *) release##ele); \
    Array_destroy(token);                           \
    break;                                          \
  }
#define releaseTokenCase(t, r)    \
  case enum_##t: {                \
    release##r(token, allocator); \
    break;                        \
  }

void releaseToken(void *token, uint32_t type, const Allocator *allocator) {
  switch (type) {
    releaseArrayCase(Entries, Entry)
    releaseArrayCase(InstrForms, InstrForm)
    releaseArrayCase(InstrParts, InstrPart)
    releaseArrayCase(MappingItems, MappingItem)
    releaseArrayCase(PatternArgs, Identifier)
    releaseArrayCase(Registers, Register)
    releaseArrayCase(SetItems, SetItem)
    releaseTokenCase(Entry, Entry)

    releaseTokenCase(Evaluable, Evaluable)
    releaseTokenCase(Immediate, Immediate)
    releaseTokenCase(InstrForm, InstrForm)
    releaseTokenCase(InstrPart, InstrPart)
    releaseTokenCase(Instruction, Instruction)
    releaseTokenCase(Layout, Layout)
    releaseTokenCase(Machine, Machine)
    releaseTokenCase(MappingItem, MappingItem)
    releaseTokenCase(MemItem, MemItem)
    releaseTokenCase(Memory, Memory)
    releaseTokenCase(Pattern, Pattern)
    releaseTokenCase(Register, Register)
    releaseTokenCase(RegisterGroup, RegisterGroup)
    releaseTokenCase(Set, Set)
    case enum_IDENTIFIER: {
      releaseIdentifier(token, allocator);
      allocator->free(token);
      break;
    }
    case enum_BIT_FIELD: {
      releaseBitField(token, allocator);
      allocator->free(token);
      break;
    }
  }
}
