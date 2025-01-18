/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: define.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_DEFINE_H
#define MACHINE_DEFINE_H

#include "context.h"

void gen_memory_dec(GContext *context, Array *buffer, const Memory *mem);
void gen_memory_def(GContext *context, Array *buffer, const Memory *mem);
void gen_memory_enum_item(GContext *context, Array *buffer, const Memory *mem);

void gen_immediate_dec(GContext *context, Array *buffer, const Immediate *imm);
void gen_immediate_def(GContext *context, Array *buffer, const Immediate *imm);
void gen_immediate_enum_item(GContext *context, Array *buffer, const Immediate *imm);

void gen_register_dec(GContext *context, Array *buffer, const Register *reg);
void gen_register_def(GContext *context, Array *buffer, const Register *reg);
void gen_register_enum_item(GContext *context, Array *buffer, const Register *reg);

#endif  // MACHINE_DEFINE_H
