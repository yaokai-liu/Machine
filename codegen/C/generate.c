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
#include "char_t.h"
#include "context.h"
#include "define.h"
#include "encoding.h"
#include "generator.h"
#include "pattern_match.h"
#include "static.h"
#include "target.h"
#include "tokens.gen.h"
#include <stdint.h>

Generator *Generator_new(const Allocator *allocator) {
  Generator *generator = allocator->calloc(1, sizeof(Generator));
  generator->allocator = allocator;
  return generator;
}
void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *headpath, const char_t *libpath,
    const char_t *cr_holder, const char_t *year
) {
  generator->outname = outname;
  generator->headpath = headpath;
  generator->libpath = libpath;
  generator->cr_holder = cr_holder;
  generator->year = year;
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
