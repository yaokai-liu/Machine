/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: pattern_match.c
 * Creator: Yaokai Liu
 * Create Date: 2025-01-19
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "pattern_match.h"
#include "array.h"
#include "encoding.h"
#include "generate.h"

void gen_pattern_match(Generator *generator, const Machine *machine) {
  GContext *context = machine->context;
  gen_jump_table_dec(generator, machine);
  Array *key_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  Array *state_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  gen_jump_table_def(generator, machine, key_buffer, state_buffer);
  Array *buffer = Generator_getOutputBuffer(generator, CtxBuf_definitions);
  Array_concat(buffer, key_buffer);
  Array_concat(buffer, state_buffer);
  releasePrimeArray(key_buffer);
  releasePrimeArray(state_buffer);
}
