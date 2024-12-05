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
#include "target.h"
#include "terminal.h"

typedef struct Namespace Namespace;

typedef struct GContext GContext;

GContext *GContext_new(const Allocator *allocator);

void GContext_addOpcode(GContext *context, Identifier *ident, Instruction *instr);

void GContext_addPattern(GContext *context, Identifier *ident, Entry *entry);

void *GContext_findOpcode(GContext *context, Identifier *ident);

void *GContext_findPattern(GContext *context, Identifier *ident);

void GContext_destroy(GContext *context, const Allocator *allocator);

#endif  // MACHINE_CONTEXT_H
