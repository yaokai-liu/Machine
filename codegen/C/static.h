/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: static.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_STATIC_H
#define MACHINE_STATIC_H

#include "generate.h"

void gen_header(
    GContext *context, Array *buffer, char_t *filename, int32_t year, char_t *cr_holder
);
void gen_export_header(Generator *generator, const Machine *machine);
void gen_export_tail(Generator *generator, const Machine *machine);
void gen_static_definitions(Generator *generator, const Machine *machine);
void gen_driver(Generator *generator, const Machine *machine);

#endif  // MACHINE_STATIC_H
