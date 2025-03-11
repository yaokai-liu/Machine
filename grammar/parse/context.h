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
 * Filename: context.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_CONTEXT_H
#define MACHINE_CONTEXT_H

#include "allocator.h"
#include "stack.h"
#include "target.h"
#include "terminal.h"
#include "trie.h"

typedef struct Record {
  uint32_t typeid;
  uint32_t offset;
} Record;

typedef struct ParseContext {
  const Allocator *allocator;
  Array /*<Register>*/ *regArray;
  Array /*<Immediate>*/ *immArray;
  Array /*<Memory>*/ *memArray;
  Array /*<Set>*/ *setArray;
  Array /*<RegisterGroup>*/ *grpArray;
  Array /*<Instruction>*/ *instrArray;
  Array /*<Record>*/ *recordArray;
  Array /*<TrieKeyItem>*/ *keyArray;
  Array /*<TrieNodeItem>*/ *stateArray;
  AVLTree /*<uint64_t>*/ *recordMap;
  AVLTree /*<REFER(Instruction)>*/ *opcodeMap;

  // temporary variable
  MemItems *items;
  InstrParts *parts;
  Array /*<Pattern*>*/ *patterns;
  Stack /*<uint64_t>*/ *widthStack;
  Stack /*<Identifier*>*/ *identStack;
  AVLTree /*<BitField*, MappingItem*>*/ *mappingTree;
  uint32_t maxArgCount;
  uint32_t maxFieldCount;
  const char_t *errorMessage;
} ParseContext;

typedef struct ParseContext ParseContext;

ParseContext *GContext_new(const Allocator *allocator);

const Allocator *GContext_getAllocator(const ParseContext *context);

void GContext_addOpcode(
    ParseContext *context, const REFER(Identifier) ident, REFER(Instruction) instr
);

REFER(Instruction) GContext_findOpcode(ParseContext *context, const REFER(Identifier) ident);

void GContext_addRecord(ParseContext *context, const REFER(Identifier) ident, Record *record);

const Record *GContext_findRecord(const ParseContext *context, const REFER(Identifier) ident);
const Parameter *GContext_findParameter(ParseContext *context, const REFER(Identifier) ident);

REFER(Immediate) GContext_addImmediate(ParseContext *context, const Immediate *imm);
REFER(Register) GContext_addRegister(ParseContext *context, const Register *reg);
REFER(Memory) GContext_addMemory(ParseContext *context, const Memory *mem);
REFER(RegisterGroup) GContext_addRegisterGroup(ParseContext *context, const RegisterGroup *grp);
REFER(Set) GContext_addSet(ParseContext *context, const Set *set);
REFER(Instruction) GContext_addInstruction(ParseContext *context, const Instruction *instr);

const Immediate *GContext_getImmediate(const ParseContext *context, uint32_t offset);
const Register *GContext_getRegister(const ParseContext *context, uint32_t offset);
const Memory *GContext_getMemory(const ParseContext *context, uint32_t offset);
const RegisterGroup *GContext_getRegisterGroup(const ParseContext *context, uint32_t offset);
const Instruction *GContext_getInstruction(const ParseContext *context, uint32_t index);
const Set *GContext_getSet(const ParseContext *context, uint32_t offset);

Register *GContext_referToRegister(const ParseContext *context, REFER(Register) v_reg);

Array *GContext_getPatternArray(const ParseContext *context);

void *GContext_findIdentInStack(ParseContext *context, const REFER(Identifier) ident);

void Gcontext_setParts(ParseContext *context, InstrParts *parts);

void Gcontext_setItems(ParseContext *context, MemItems *items);

const InstrPart *GContext_findInstrPart(ParseContext *context, const REFER(Identifier) ident);

const MemItem *GContext_findMemItem(ParseContext *context, const REFER(Identifier) ident);

void GContext_addPattern(ParseContext *context, Pattern *pattern);

bool GContext_testPattern(ParseContext *context, PatternArgs *patternArgs);

void GContext_addMapItem(ParseContext *context, MappingItem *item);

MappingItem *GContext_getMapItem(ParseContext *context, BitField *bf);

Trie /*<REFER(Record), uint64_t>*/ *
    GContext_build_args_trie(ParseContext *context, const Instruction *instr);
void GContext_dump_instruction(ParseContext *context, Instruction *instr);

uint64_t GContext_getLastWidth(ParseContext *context);

void GContext_setErrorMessage(ParseContext *context, const char_t *msg);
const char_t *GContext_getErrorMessage(ParseContext *context);

void GContext_destroy(ParseContext *context);

typedef void fn_ctx_act(ParseContext *context, void *token);

fn_ctx_act *get_after_stack_actions(int32_t state);
fn_ctx_act *get_after_reduce_actions(int32_t state);

#endif  // MACHINE_CONTEXT_H
