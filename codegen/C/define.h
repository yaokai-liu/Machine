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

void gen_enum_item(Generator *generator, const Machine *machine);
void gen_set_grp_jump_table(Generator *generator, const Machine *machine);

void gen_context_dec(Generator *generator, const Machine *machine);
void gen_context_def(Generator *generator, const Machine *machine);

#endif  // MACHINE_DEFINE_H
