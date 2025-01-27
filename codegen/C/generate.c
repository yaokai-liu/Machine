/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generate.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "generate.h"
#include "array.h"
#include "context.h"
#include "define.h"
#include "encoding.h"
#include "pattern_match.h"
#include "static.h"
#include "target.h"
#include "tokens.gen.h"
#include <stdint.h>

typedef struct Generator {
  const Allocator *allocator;
  Array /*<char_t>*/ *buffers[16];
} Generator;

Generator *Generator_new(const Allocator *allocator) {
  Generator *generator = allocator->calloc(1, sizeof(Generator));
  generator->allocator = allocator;
  return generator;
}

inline Array *Generator_getOutputBuffer(Generator *generator, uint32_t index) {
  if (index >= 16) { return nullptr; }
  if (!generator->buffers[index]) {
    generator->buffers[index] = Array_new(sizeof(char_t), INT32_MAX, generator->allocator);
  }
  return generator->buffers[index];
}

void Generator_destroy(Generator *generator) {
  for (uint32_t i = 0; i < 16; i++) {
    if (generator->buffers[i]) { releasePrimeArray(generator->buffers[i]); }
  }
  generator->allocator->free(generator);
}

int32_t codegen(Generator *generator, const Machine *machine) {
  gen_export_header(generator, machine);
  gen_static_definitions(generator, machine);
  gen_context_dec(generator, machine);
  gen_context_def(generator, machine);
  gen_pattern_match(generator, machine);
  gen_enum_item(generator, machine);
  gen_instr_exec(generator, machine);
  gen_set_grp_jump_table(generator, machine);
  gen_driver(generator, machine);
  gen_export_tail(generator, machine);
  return 0;
}
