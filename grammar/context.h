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

typedef struct Namespace Namespace;

typedef struct GContext GContext;

GContext *GContext_new(const Allocator *allocator);

void GContext_setCodegen(GContext * context, codegen_t * (*getCodegen)(uint32_t token_type));

void GContext_addOpcode(GContext *context, Identifier *ident, Instruction *instr);

void *GContext_findOpcode(GContext *context, Identifier *ident);

void GContext_addRefer(GContext *context, Identifier *ident, Entry *entry);

void *GContext_findRefer(GContext *context, Identifier *ident);

void *GContext_findIdentInStack(GContext *context, Identifier *ident);

void GContext_addPattern(GContext *context, Pattern *pattern);

bool GContext_testPattern(GContext *context, PatternArgs *patternArgs);

void GContext_addMapItem(GContext *context, MappingItem *item);

MappingItem *GContext_getMapItem(GContext *context, BitField *bf);

uint64_t GContext_getLastWidth(GContext *context);

void GContext_destroy(GContext *context, const Allocator *allocator);

typedef void fn_ctx_act(GContext *context, void *token);

fn_ctx_act *get_after_stack_actions(int32_t state);
fn_ctx_act *get_after_reduce_actions(int32_t state);

#endif  // MACHINE_CONTEXT_H
