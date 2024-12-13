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
#include "target.h"
#include "terminal.h"

typedef struct Record {
  uint32_t typeid;
  uint32_t offset;
} Record;

typedef struct GContext GContext;

GContext *GContext_new(const Allocator *allocator);

const Allocator *GContext_getAllocator(GContext *context);

Array *GContext_getOutputBuffer(GContext *context);

void GContext_setCodegen(GContext *context, codegen_t *(*getCodegen)(uint32_t token_type));

codegen_t *GContext_getCodegen(GContext *context, uint32_t token_type);

void GContext_addOpcode(GContext *context, const Identifier *ident, Instruction *instr);

void *GContext_findOpcode(GContext *context, const Identifier *ident);

void GContext_addRecord(GContext *context, const Identifier *ident, Record *record);

void *GContext_findRecord(GContext *context, const Identifier *ident);

uint32_t GContext_addImmediate(GContext *context, const Immediate *imm);
uint32_t GContext_addRegister(GContext *context, const Register *reg);
uint32_t GContext_addMemory(GContext *context, const Memory *mem);
uint32_t GContext_addRegisterGroup(GContext *context, const RegisterGroup *grp);
uint32_t GContext_addSet(GContext *context, const Set *set);

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
