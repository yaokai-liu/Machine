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
#include "codegen.h"
#include "stack.h"
#include "target.h"
#include "terminal.h"
#include "trie.h"

typedef struct Record {
  uint32_t typeid;
  uint32_t offset;
} Record;

enum Ctx_ByteBuffer {
  CtxBuf_enum_item,
  CtxBuf_encoding_def,
  CtxBuf_encoding_dec,
  CtxBuf_register_dec,
  CtxBuf_memory_dec,
  CtxBuf_immediate_dec,
  CtxBuf_register_def,
  CtxBuf_memory_def,
  CtxBuf_immediate_def,
};

typedef struct GContext {
  const Allocator *allocator;
  Array /*<Register>*/ *regArray;
  Array /*<Immediate>*/ *immArray;
  Array /*<Memory>*/ *memArray;
  Array /*<Set>*/ *setArray;
  Array /*<RegisterGroup>*/ *grpArray;
  Array /*<Record>*/ *recordArray;
  Trie /*<uint32_t>*/ *objectMap;
  Trie /*<uint32_t>*/ *opcodeMap;
  codegen_t *(*getCodegen)(uint32_t token_type);

  Array *outputs[16];

  // temporary variable
  Array *patterns;
  Stack *widthStack;
  Stack *identStack;
  AVLTree *mappingTree;
} GContext;

typedef struct GContext GContext;

GContext *GContext_new(const Allocator *allocator);

const Allocator *GContext_getAllocator(GContext *context);

Array *GContext_getOutputBuffer(GContext *context, uint32_t index);

void GContext_setCodegen(GContext *context, codegen_t *(*getCodegen)(uint32_t token_type));

codegen_t *GContext_getCodegen(GContext *context, uint32_t token_type);

void GContext_addOpcode(GContext *context, const Identifier *ident, Instruction *instr);

void *GContext_findOpcode(GContext *context, const Identifier *ident);

void GContext_addRecord(GContext *context, const Identifier *ident, Record *record);

void *GContext_findRecord(GContext *context, const Identifier *ident);

REFER(Immediate) GContext_addImmediate(GContext *context, const Immediate *imm);
REFER(Register) GContext_addRegister(GContext *context, const Register *reg);
REFER(Memory) GContext_addMemory(GContext *context, const Memory *mem);
REFER(RegisterGroup) GContext_addRegisterGroup(GContext *context, const RegisterGroup *grp);
REFER(Set) GContext_addSet(GContext *context, const Set *set);

Immediate *GContext_getImmediate(GContext *context, uint32_t offset);
Register *GContext_getRegister(GContext *context, uint32_t offset);
Memory *GContext_getMemory(GContext *context, uint32_t offset);
RegisterGroup *GContext_getRegisterGroup(GContext *context, uint32_t offset);
Set *GContext_getSet(GContext *context, uint32_t offset);

void *GContext_findIdentInStack(GContext *context, Identifier *ident);

void GContext_addPattern(GContext *context, Pattern *pattern);

bool GContext_testPattern(GContext *context, PatternArgs *patternArgs);

void GContext_addMapItem(GContext *context, MappingItem *item);

MappingItem *GContext_getMapItem(GContext *context, BitField *bf);

uint64_t GContext_getLastWidth(GContext *context);

void GContext_destroy(GContext *context);

typedef void fn_ctx_act(GContext *context, void *token);

fn_ctx_act *get_after_stack_actions(int32_t state);
fn_ctx_act *get_after_reduce_actions(int32_t state);

#endif  // MACHINE_CONTEXT_H
