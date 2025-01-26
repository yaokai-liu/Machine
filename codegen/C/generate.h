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
#include "context.h"
#include <stdbool.h>
#include <stdint.h>

enum Ctx_ByteBuffer {
  CtxBuf_exports,
  CtxBuf_includes,
  CtxBuf_macros,
  CtxBuf_enums,
  CtxBuf_types,
  CtxBuf_declares,
  CtxBuf_definitions,
  CtxBuf_tables
};

typedef struct Generator Generator;
Generator *Generator_new(const Allocator *allocator);
Array *Generator_getOutputBuffer(Generator *generator, uint32_t index);
void Generator_destroy(Generator *generator);
int32_t codegen(Generator *generator, const Machine *machine);

#endif  // MACHINE_GENERATE_H
