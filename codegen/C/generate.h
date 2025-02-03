/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generate.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GENERATE_H
#define MACHINE_GENERATE_H

#include "array.h"
#include "char_t.h"
#include "context.h"
#include <stdbool.h>
#include <stdint.h>

enum Ctx_ByteBuffer {
  GenBuf_exports,
  GenBuf_includes,
  GenBuf_macros,
  GenBuf_enums,
  GenBuf_types,
  GenBuf_declares,
  GenBuf_definitions,
  GenBuf_tables
};

typedef struct Generator Generator;
Generator *Generator_new(const Allocator *allocator);
void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *headpath, const char_t *libpath,
    const char_t *cr_holder, const char_t *year
);
Array *Generator_getOutputBuffer(Generator *generator, uint32_t index);
void Generator_destroy(Generator *generator);
int32_t codegen(Generator *generator, const Machine *machine);

#endif  // MACHINE_GENERATE_H
