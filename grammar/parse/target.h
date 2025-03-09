/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Project Name: machine
 * Module Name: grammar/parse
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

#define REFER(T) /*VirtAddr*/ T *

typedef struct Entry {
  uint32_t type;
  REFER(void) target;
} Entry;

typedef Array Entries;  // Array<Entry>

typedef struct ParseContext ParseContext;

typedef struct Machine {
  Identifier *name;
  ParseContext *context;
} Machine;

typedef struct Immediate {
  Identifier *name;
  uint32_t width;
  uint32_t type;
} Immediate;

typedef struct Parameter {
  Identifier *type;
  Identifier *name;
  uint32_t used;
} Parameter;

typedef Array PatternArgs;  // Array<Parameter>

typedef struct Pattern {
  PatternArgs *args;
} Pattern;

typedef struct Evaluable {
  uint32_t type;
  void *lhs;
  void *rhs;
} Evaluable, Variable;

struct Expression {
  uint32_t type;
  void *lhs;
  void *rhs;
};
typedef struct Expression Expr, CondExpr, AndCondExpr, SingleCondExpr;
typedef struct Expression Arith_0_Expr, Arith_1_Expr, Arith_2_Expr, Arith_3_Expr;

typedef struct Condition {
  CondExpr *expr;
} Condition;

typedef Condition FormCheck;

typedef Array Options;  // Array<Evaluable>

typedef struct Switchable {
  CondExpr *expr;
  Options *options;
} Switchable;

typedef struct MappingItem {
  uint32_t type;
  BitField *field;
  void *target;
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
  Identifier *name;
  uint32_t width;
  Layout *layout;
  Condition *condition;
} InstrPart;

typedef Array InstrParts;  // Array<InstrPart>

typedef struct InstrForm {
  uint32_t width;
  uint32_t tick;
  Pattern *pattern;
  FormCheck *check;
  InstrParts *parts;
} InstrForm;

typedef Array InstrForms;  // Array<InstrForm>

typedef struct Instruction {
  Identifier *name;
  InstrForms *forms;
  uint32_t entry_offset;
} Instruction;

typedef struct MemItem {
  Identifier *name;
  uint32_t start;
  uint32_t width;
  Identifier *type;
} MemItem;

typedef Array MemItems;  // Array<MemItem>

typedef struct Memory {
  Identifier *name;
  uint32_t width;
  MemItems *items;
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

typedef Array SetItems;  // Array<Identifier>

typedef struct Set {
  Identifier *name;
  SetItems *items;
} Set;

#include "allocator.h"

void releaseToken(void *token, uint32_t type, const Allocator *allocator);

void releaseEntry(Entry *entry, const Allocator *allocator);
void releaseMachine(Machine *machine, const Allocator *allocator);
void releaseImmediate(Immediate *immediate, const Allocator *allocator);
void releaseParameter(Parameter *parameter, const Allocator *allocator);
void releasePattern(Pattern *pattern, const Allocator *allocator);
void releaseVariable(Variable *variable, const Allocator *allocator);
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
void releaseSet(Set *set, const Allocator *allocator);
void releaseCondition(Condition *condition, const Allocator *allocator);
void releaseExpr(CondExpr *expr, const Allocator *allocator);

int32_t PatternArgs_cmp(PatternArgs *args1, PatternArgs *args2);

#endif  // MACHINE_TARGET_H
