/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TARGET_H
#define MACHINE_TARGET_H

#include "array.h"
#include "avl-tree.h"
#include "terminal.h"

#define REFER(T) /*VirtAddr*/T *

typedef struct Entry {
  uint32_t type;
  REFER(void) target;
} Entry;

typedef Array Entries;  // Array<Entry>

typedef struct Machine {
  Identifier *name;
  Entries *entries;
  void *context;
} Machine;

typedef struct Immediate {
  Identifier *name;
  uint32_t width;
  uint32_t type;
} Immediate;

typedef Array PatternArgs;  // Array<Identifier>

typedef struct Pattern {
  Identifier *name;
  PatternArgs *args;
} Pattern;

typedef struct Evaluable {
  uint32_t type;
  void *lhs;
  void *rhs;
} Evaluable;

typedef struct MappingItem {
  BitField *field;
  Evaluable *evaluable;
} MappingItem;

typedef struct MappingItems {
  AVLTree *itemTree;
  Array /*<MappingItem>*/ *itemArray;
  uint32_t lowest;
  Evaluable *default_eval;
} MappingItems;

typedef struct Layout {
  uint32_t type;
  void *target;
} Layout;

typedef struct InstrPart {
  uint32_t type;
  uint32_t width;
  Layout *layout;
} InstrPart;

typedef Array InstrParts;  // Array<InstrPart>

typedef struct InstrForm {
  uint32_t width;
  uint32_t tick;
  Pattern *pattern;
  struct {
    uint32_t width;
    Layout *layout;
  } parts[3];
} InstrForm;

typedef Array InstrForms;  // Array<InstrForm>

typedef struct Instruction {
  Identifier *name;
  InstrForms *forms;
} Instruction;

typedef struct MemItem {
  uint32_t type;
  BitField *field;
} MemItem;

typedef struct Memory {
  Identifier *name;
  uint32_t width;
  BitField *base;
  BitField *offset;
} Memory;

typedef struct RegisterGroup RegisterGroup;

typedef struct Register {
  Identifier *name;
  BitField *field;
  uint64_t code;
  REFER(RegisterGroup) group;
} Register;

typedef Array Registers;  // Array<REFER(Register)>

typedef struct RegisterGroup {
  Identifier *name;
  uint32_t width;
  Registers *registers;
} RegisterGroup;

typedef Array SetItems;  // Array<SetItem>

typedef struct SetItem {
  Identifier *name;
} SetItem;

typedef struct Set {
  Identifier *name;
  SetItems *items;
} Set;

#include "allocator.h"

void releaseToken(void *token, uint32_t type, const Allocator *allocator);

void releaseEntry(Entry *entry, const Allocator *allocator);
void releaseMachine(Machine *machine, const Allocator *allocator);
void releaseImmediate(Immediate *immediate, const Allocator *allocator);
void releasePattern(Pattern *pattern, const Allocator *allocator);
void releaseEvaluable(Evaluable *evaluable, const Allocator *allocator);
void releaseMappingItem(MappingItem *item, const Allocator *allocator);
void releaseMappingItems(MappingItems *items, const Allocator *allocator);
void releaseLayout(Layout *layout, const Allocator *allocator);
void releaseInstrPart(InstrPart *part, const Allocator *allocator);
void releaseInstrForm(InstrForm *form, const Allocator *allocator);
void releaseInstruction(Instruction *instr, const Allocator *allocator);
void releaseMemItem(MemItem *item, const Allocator *allocator);
void releaseMemory(Memory *memory, const Allocator *allocator);
void releaseRegister(Register *reg, const Allocator *allocator);
void releaseRegisterGroup(RegisterGroup *rg, const Allocator *allocator);
void releaseSetItem(SetItem *item, const Allocator *allocator);
void releaseSet(Set *set, const Allocator *allocator);

int32_t Identifier_cmp(const Identifier *ident1, const Identifier *ident2);
int32_t PatternArgs_cmp(PatternArgs *args1, PatternArgs *args2);
int32_t BitField_cmp(BitField *bf1, BitField *bf2);

#endif  // MACHINE_TARGET_H
