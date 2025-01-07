/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: reduce.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "action-table.h"
#include "array.h"
#include "avl-tree.h"
#include "codegen.h"
#include "context.h"
#include "enum.h"
#include "semantic.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"
#include <stdint.h>

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define grammarAssertDefinedRecord(ident)                 \
  do {                                                    \
    Record *record = GContext_findRecord(context, ident); \
    if (!record) { return nullptr; }                      \
  } while (false)

#define grammarAssertNotDeclaredRecord(ident)             \
  do {                                                    \
    Record *record = GContext_findRecord(context, ident); \
    if (record) { return nullptr; }                       \
    void *id = GContext_findIdentInStack(context, ident); \
    if (id) { return nullptr; }                           \
  } while (false)

#define grammarAssert(bool_expr) do { \
  if (!(bool_expr)) { return nullptr; } \
} while (false)

Entries *p_Entries_0(void *argv[], GContext *, const Allocator *allocator) {
  Entries *entries = (Entries *) argv[0];
  Entry *entry = (Entry *) argv[1];
  Array_append(entries, entry, 1);
  allocator->free(entry);
  return entries;
}

Entries *p_Entries_1(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = (Entry *) argv[0];
  Entries *entries = Array_new(sizeof(Entry), enum_Entry, allocator);
  Array_append(entries, entry, 1);
  allocator->free(entry);
  return entries;
}

Entry *p_Entry_0(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_RegisterGroup;
  entry->target = (RegisterGroup *) argv[0];
  return entry;
}

Entry *p_Entry_1(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Instruction;
  entry->target = (Instruction *) argv[0];
  return entry;
}

Entry *p_Entry_2(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Memory;
  entry->target = (Memory *) argv[0];
  return entry;
}

Entry *p_Entry_3(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Immediate;
  entry->target = (Immediate *) argv[0];
  return entry;
}

Entry *p_Entry_4(void *argv[], GContext *, const Allocator *allocator) {
  Entry *entry = allocator->calloc(1, sizeof(Entry));
  entry->type = enum_Set;
  entry->target = (Set *) argv[0];
  return entry;
}

Evaluable *p_Evaluable_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0];
  void *rhs = argv[2];

  grammarAssertDefinedRecord(lhs);

  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_MEM_KEY;
  evaluable->lhs = lhs;
  evaluable->rhs = rhs;
  return evaluable;
}

Evaluable *p_Evaluable_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0];
  BitField *rhs = (BitField *) argv[1];

  grammarAssertDefinedRecord(lhs);

  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_BIT_FIELD;
  evaluable->lhs = lhs;
  evaluable->rhs = rhs;
  return evaluable;
}

Evaluable *p_Evaluable_2(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];

  grammarAssertDefinedRecord(ident);

  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_IDENTIFIER;
  evaluable->lhs = ident;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *p_Evaluable_3(void *argv[], GContext *, const Allocator *allocator) {
  uint64_t number = (uint64_t) argv[0];
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) number;
  evaluable->rhs = nullptr;
  return evaluable;
}

Immediate *p_Immediate_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[1];

  grammarAssertNotDeclaredRecord(ident);

  uint32_t width = (uint32_t) (uint64_t) argv[2];
  uint32_t type = (uint64_t) argv[3];
  Immediate *imm = allocator->calloc(1, sizeof(Immediate));
  imm->type = type;
  imm->width = width;
  imm->name = ident;

  GContext_addImmediate(context, imm);
  allocator->free(imm);

  return imm;
}

InstrForm *p_InstrForm_0(void *argv[], GContext *context, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  InstrParts *part_array = (InstrParts *) argv[4];

  uint32_t n_parts = Array_length(part_array);
  if (Array_length(part_array) > 3) { return nullptr; }

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = 1;
  form->pattern = pattern;

  const InstrPart * const parts = Array_real_addr(part_array, 0);
  for (uint32_t i = 0; i < n_parts; i++) {
    const InstrPart *part = &parts[i];
    if (form->parts[part->type].layout) {
      allocator->free(form);
      return nullptr;
    }
    form->parts[part->type].width = part->width;
    form->parts[part->type].layout = part->layout;
  }
  releasePrimeArray(part_array);

  codegen_t *fn_codegen = GContext_getCodegen(context, enum_InstrForm);
  if (fn_codegen) {
    Array *buffer = Array_new(sizeof(char_t), -2, allocator);
    fn_codegen(context, form, buffer);
  }
  return form;
}

InstrForm *p_InstrForm_1(void *argv[], GContext *context, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  uint32_t tick = (uint32_t) (uint64_t) argv[3];
  InstrParts *part_array = (InstrParts *) argv[5];

  uint32_t n_parts = Array_length(part_array);
  if (Array_length(part_array) > 3) { return nullptr; }

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = tick;
  form->pattern = pattern;

  const InstrPart * const parts = Array_real_addr(part_array, 0);
  for (uint32_t i = 0; i < n_parts; i++) {
    const InstrPart *part = &parts[i];
    if (form->parts[part->type - 1].layout) {
      allocator->free(form);
      return nullptr;
    }
    form->parts[part->type - 1].width = part->width;
    form->parts[part->type - 1].layout = part->layout;
  }
  releasePrimeArray(part_array);

  codegen_t *fn_codegen = GContext_getCodegen(context, enum_InstrForm);
  if (fn_codegen) {
    Array *buffer = GContext_getOutputBuffer(context);
    fn_codegen(context, form, buffer);
  }
  return form;
}

InstrForms *p_InstrForms_0(void *argv[], GContext *, const Allocator *allocator) {
  InstrForms *forms = (InstrForms *) argv[0];
  InstrForm *form = (InstrForm *) argv[1];
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrForms *p_InstrForms_1(void *argv[], GContext *, const Allocator *allocator) {
  InstrForm *form = (InstrForm *) argv[0];
  InstrForms *forms = Array_new(sizeof(InstrForm), enum_InstrForm, allocator);
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrPart *p_InstrPart_0(void *argv[], GContext *, const Allocator *allocator) {
  enum PART_KEY key = (uint32_t) (uint64_t) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Layout *layout = (Layout *) argv[4];

  if (0 == width) { return nullptr; }

  InstrPart *part = allocator->calloc(1, sizeof(InstrPart));
  part->type = key;
  part->width = width;
  part->layout = layout;
  return part;
}

InstrParts *p_InstrParts_0(void *argv[], GContext *, const Allocator *allocator) {
  InstrParts *parts = (InstrParts *) argv[0];
  InstrPart *part = (InstrPart *) argv[1];
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

InstrParts *p_InstrParts_1(void *argv[], GContext *, const Allocator *allocator) {
  InstrPart *part = (InstrPart *) argv[0];
  InstrParts *parts = Array_new(sizeof(InstrPart), enum_InstrPart, allocator);
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

Instruction *p_Instruction_0(void *argv[], GContext *, const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  InstrForms *forms = (InstrForms *) argv[3];
  Instruction *instr = allocator->calloc(1, sizeof(Instruction));
  instr->name = identifier;
  instr->forms = forms;
  return instr;
}

Layout *p_Layout_0(void *argv[], GContext *, const Allocator *allocator) {
  Evaluable *evaluable = (Evaluable *) argv[0];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Evaluable;
  layout->target = evaluable;
  return layout;
}

Layout *p_Layout_1(void *argv[], GContext *, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[1];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_MappingItems;
  layout->target = items;
  return layout;
}

Machine *p_Machine_0(void *argv[], GContext *, const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  Entries *entries = argv[3];
  Machine *machine = allocator->calloc(1, sizeof(Machine));
  machine->name = identifier;
  machine->entries = entries;
  return machine;
}

Machine *p___EXTEND_RULE__(void *argv[], const Allocator *) {
  return (Machine *) argv[0];
}

MappingItem *p_MappingItem_0(void *argv[], GContext *context, const Allocator *allocator) {
  BitField *bit_field = (BitField *) argv[0];
  Evaluable *evaluable = (Evaluable *) argv[2];

  if (bit_field) {
    uint64_t width = GContext_getLastWidth(context);
    if (bit_field->upper > width) { return nullptr; }
  }
  if (GContext_getMapItem(context, bit_field)) { return nullptr; }
  if (0 != check_mapping_item(context, bit_field, evaluable)) { return nullptr; }

  MappingItem *item = allocator->calloc(1, sizeof(MappingItem));
  item->field = bit_field;
  item->evaluable = evaluable;

  GContext_addMapItem(context, item);

  return item;
}

MappingItems *p_MappingItems_0(void *argv[], GContext *, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[0];
  MappingItem *item = (MappingItem *) argv[2];

  if (!item->field) {
    if (items->default_eval) { return nullptr; }
    items->default_eval = item->evaluable;
  } else {
    items->lowest = min(item->field->lower, items->lowest);
    uint32_t index = Array_length(items->itemArray);
    AVLTree_set(items->itemTree, (uint64_t) item->field, (void *) (uint64_t) index + 1);
    Array_append(items->itemArray, item, 1);
  }
  allocator->free(item);
  return items;
}

MappingItems *p_MappingItems_1(void *argv[], GContext *, const Allocator *allocator) {
  MappingItem *item = (MappingItem *) argv[0];

  MappingItems *items = allocator->calloc(1, sizeof(MappingItems));
  items->itemArray = Array_new(sizeof(MappingItem), enum_MappingItem, allocator);
  items->itemTree = AVLTree_new(allocator, (compare_t *) BitField_cmp);
  if (!item->field) {
    items->default_eval = item->evaluable;
    items->lowest = 0;
  } else {
    Array_append(items->itemArray, item, 1);
    AVLTree_set(items->itemTree, (uint64_t) item->field, (void *) 1);
    items->lowest = item->field->lower;
  }
  allocator->free(item);
  return items;
}

MemItem *p_MemItem_0(void *argv[], GContext *, const Allocator *allocator) {
  enum MEM_KEY key = (uint32_t) (uint64_t) argv[0];
  if (key != MEM_BASE && key != MEM_OFFSET) { return nullptr; }
  BitField *bit_field = (BitField *) argv[2];
  MemItem *item = allocator->calloc(1, sizeof(MemItem));
  item->type = key;
  item->field = bit_field;
  return item;
}

Memory *p_Memory_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[1];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  MemItem *item1 = (MemItem *) argv[4];
  MemItem *item2 = (MemItem *) argv[5];

  grammarAssertNotDeclaredRecord(ident);

  if (item1->type == item2->type) { return nullptr; }
  Memory *mem = allocator->calloc(1, sizeof(Memory));
  mem->name = ident;
  mem->width = width;
  if (item1->type == MEM_BASE) {
    mem->base = item1->field;
    mem->offset = item2->field;
  } else {
    mem->base = item2->field;
    mem->offset = item1->field;
  }
  allocator->free(item1);
  allocator->free(item2);

  GContext_addMemory(context, mem);
  allocator->free(mem);

  return mem;
}

Pattern *p_Pattern_0(void *argv[], GContext *context, const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[1];

  if (GContext_testPattern(context, args)) { return nullptr; }

  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = args;

  GContext_addPattern(context, pattern);

  return pattern;
}

PatternArgs *p_PatternArgs_0(void *argv[], GContext *context, const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[0];
  Identifier *ident = (Identifier *) argv[2];

  grammarAssertDefinedRecord(ident);

  Array_append(args, ident, 1);
  allocator->free(ident);
  return args;
}

PatternArgs *p_PatternArgs_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];

  grammarAssertDefinedRecord(ident);

  PatternArgs *args = Array_new(sizeof(Identifier), enum_PatternArgs, allocator);
  Array_append(args, ident, 1);
  allocator->free(ident);

  return args;
}

Register *p_Register_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[0];
  BitField *field = (BitField *) argv[2];
  uint64_t code = (uint64_t) argv[4];

  grammarAssertNotDeclaredRecord(ident);

  Register reg = {.name = ident, .field = field, .code = code};
  return GContext_addRegister(context, &reg);
}

RegisterGroup *p_RegisterGroup_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Registers *registers = (Registers *) argv[4];

  grammarAssertNotDeclaredRecord(ident);

  const uint32_t len = Array_length(registers);
  grammarAssert(len > 0);

  RegisterGroup grp = { .name = ident, .width = width, .registers = registers };
  RegisterGroup *result = GContext_addRegisterGroup(context, &grp);
  Register *regs = Array_real_addr(registers, 0);
  for (uint32_t i = 0; i < len; i ++) {
    regs[i].group = result;
  }
  return result;
}

Registers *p_Registers_0(void *argv[], GContext *, const Allocator *) {
  Registers *regs = (Registers *) argv[0];
  Register *reg = (Register *) argv[1];
  Array_append(regs, &reg, 1);
  return regs;
}

Registers *p_Registers_1(void *argv[], GContext *, const Allocator *allocator) {
  Register *reg = argv[0];
  Registers *regs = Array_new(sizeof(REFER(Register)), -1, allocator);
  Array_append(regs, &reg, 1);
  return regs;
}

Set *p_Set_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];
  SetItems *items = (SetItems *) argv[1];

  grammarAssertNotDeclaredRecord(ident);

  Set *set = allocator->calloc(1, sizeof(Set));
  set->name = ident;
  set->items = items;

  GContext_addSet(context, set);
  allocator->free(set);

  return set;
}

SetItems *p_SetItems_0(void *argv[], GContext *context, const Allocator *) {
  SetItems *items = (SetItems *) argv[0];
  Identifier *ident = (Identifier *) argv[2];

  grammarAssertDefinedRecord(ident);

  SetItem item = {ident};
  Array_append(items, &item, 1);

  return items;
}

SetItems *p_SetItems_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];

  grammarAssertDefinedRecord(ident);

  SetItem item = {ident};
  SetItems *items = Array_new(sizeof(SetItem), enum_SetItems, allocator);
  Array_append(items, &item, 1);

  return items;
}

#include "stack.h"

void releaseToken(void *token, uint32_t type, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *token, uint32_t type, const Allocator *allocator
);

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *args[], int32_t *states, uint32_t argc,
    const Allocator *allocator
);

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *token, uint32_t type, const Allocator *allocator
) {
  int32_t state = 0;
  Stack_top(state_stack, (&state), sizeof(int32_t));
  releaseToken(token, type, allocator);
  allocator->free(token);
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *args[], int32_t *states, uint32_t argc,
    const Allocator *allocator
) {
  for (uint32_t i = 0; i < argc; i++) {
    uint32_t type = stateCurrentTokenType(states[i]);
    releaseToken(args[i], type, allocator);
  }
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator) {
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

#define releaseArrayCase(array, ele)                 \
  case enum_##array: {                               \
    Array_reset(token, (destruct_t *) release##ele); \
    Array_destroy(token);                            \
    break;                                           \
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
    releaseTokenCase(MappingItems, MappingItems)
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
