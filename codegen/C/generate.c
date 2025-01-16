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
#include "codegen.h"
#include "context.h"
#include "encoding.h"
#include "target.h"
#include "tokens.gen.h"
#include <stdint.h>

int32_t codegen_instruction(GContext *context, Instruction *instr) {
  const InstrForm *forms = Array_real_addr(instr->forms, 0);
  const uint32_t n_forms = Array_length(instr->forms);

  Array *buffer;

  buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_def);
  gen_instr_encoding_def(context, buffer, instr->name->ptr, forms, n_forms);

  buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_dec);
  gen_instr_encoding_dec(context, buffer, instr->name->ptr, forms, n_forms);

  return 0;
}

codegen_t *get_codegen(uint32_t type) {
  switch (type) {
    case enum_Instruction: {
      return (codegen_t *) codegen_instruction;
    }
  }
  return nullptr;
}
