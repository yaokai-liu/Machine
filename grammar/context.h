/**
 * Project Name: machine
 * Module Name: grammar
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

typedef struct GContext {
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
  Trie /*<char_t, uint64_t>*/ *objectMap;
  Trie /*<char_t, REFER(Instruction)>*/ *opcodeMap;

  // temporary variable
  MemItems *items;
  InstrParts *parts;
  Array /*<Pattern*>*/ *patterns;
  Stack /*<uint64_t>*/ *widthStack;
  Stack /*<Identifier*>*/ *identStack;
  AVLTree /*<BitField*, MappingItem*>*/ *mappingTree;
  uint32_t maxArgCount;
  const char_t *errorMessage;
  bool isCondition;
} GContext;

typedef struct GContext GContext;

GContext *GContext_new(const Allocator *allocator);

const Allocator *GContext_getAllocator(const GContext *context);

void GContext_addOpcode(GContext *context, const Identifier *ident, Instruction *instr);

Instruction *GContext_findOpcode(GContext *context, const Identifier *ident);

void GContext_addRecord(GContext *context, const Identifier *ident, Record *record);

const Record *GContext_findRecord(const GContext *context, const Identifier *ident);
const Parameter *GContext_findParameter(GContext *context, Identifier *ident);

REFER(Immediate) GContext_addImmediate(GContext *context, const Immediate *imm);
REFER(Register) GContext_addRegister(GContext *context, const Register *reg);
REFER(Memory) GContext_addMemory(GContext *context, const Memory *mem);
REFER(RegisterGroup) GContext_addRegisterGroup(GContext *context, const RegisterGroup *grp);
REFER(Set) GContext_addSet(GContext *context, const Set *set);
REFER(Instruction) GContext_addInstruction(GContext *context, const Instruction *instr);

const Immediate *GContext_getImmediate(const GContext *context, uint32_t offset);
const Register *GContext_getRegister(const GContext *context, uint32_t offset);
const Memory *GContext_getMemory(const GContext *context, uint32_t offset);
const RegisterGroup *GContext_getRegisterGroup(const GContext *context, uint32_t offset);
const Instruction *GContext_getInstruction(const GContext *context, uint32_t index);
const Set *GContext_getSet(const GContext *context, uint32_t offset);

void *GContext_findIdentInStack(GContext *context, Identifier *ident);

void Gcontext_setParts(GContext *context, InstrParts *parts);

void Gcontext_setItems(GContext *context, MemItems *items);

const InstrPart *GContext_findInstrPart(GContext *context, Identifier *ident);

const MemItem *GContext_findMemItem(GContext *context, Identifier *ident);

void GContext_addPattern(GContext *context, Pattern *pattern);

bool GContext_testPattern(GContext *context, PatternArgs *patternArgs);

void GContext_addMapItem(GContext *context, MappingItem *item);

MappingItem *GContext_getMapItem(GContext *context, BitField *bf);

Trie /*<REFER(Record), uint64_t>*/ *
    GContext_build_args_trie(GContext *context, const Instruction *instr);
void GContext_dump_instruction(GContext *context, Instruction *instr);

uint64_t GContext_getLastWidth(GContext *context);
bool GContext_isCondition(GContext *context);

void GContext_setErrorMessage(GContext *context, const char_t *msg);
const char_t *GContext_getErrorMessage(GContext *context);

void GContext_destroy(GContext *context);

typedef void fn_ctx_act(GContext *context, void *token);

fn_ctx_act *get_after_stack_actions(int32_t state);
fn_ctx_act *get_after_reduce_actions(int32_t state);

#endif  // MACHINE_CONTEXT_H
