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

void gen_pattern_match(GContext *context, const Machine *machine) {
  Array *key_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_jump_table_key);
  Array *state_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_jump_table_state);
  gen_instr_encoding_mat(context, machine, key_buffer, state_buffer);
}
