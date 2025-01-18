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
#include "define.h"
#include "encoding.h"
#include "target.h"
#include "tokens.gen.h"
#include <stdint.h>

int32_t codegen_memory(GContext *context, REFER(Memory) mem) {
  mem = Array_vert2real(context->memArray, mem);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_memory_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_memory_def);
  Array *enum_buffer = GContext_getOutputBuffer(context, CtxBuf_enum_item);

  gen_memory_dec(context, dec_buffer, mem);
  gen_memory_def(context, def_buffer, mem);
  gen_memory_enum_item(context, enum_buffer, mem);

  return 0;
}

int32_t codegen_immediate(GContext *context, REFER(Immediate) imm) {
  imm = Array_vert2real(context->immArray, imm);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_immediate_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_immediate_def);
  Array *enum_buffer = GContext_getOutputBuffer(context, CtxBuf_enum_item);

  gen_immediate_dec(context, dec_buffer, imm);
  gen_immediate_def(context, def_buffer, imm);
  gen_immediate_enum_item(context, enum_buffer, imm);

  return 0;
}

int32_t codegen_register_group(GContext *context, REFER(RegisterGroup) grp) {
  grp = Array_vert2real(context->grpArray, grp);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_register_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_register_def);
  Array *enum_buffer = GContext_getOutputBuffer(context, CtxBuf_enum_item);

  const uint32_t n_regs = Array_length(grp->registers);
  REFER(Register) *regs = Array_real_addr(grp->registers, 0);
  for (uint32_t i = 0; i < n_regs; i++) {
    const Register *reg = Array_vert2real(context->regArray, regs[i]);
    gen_register_dec(context, dec_buffer, reg);
    gen_register_def(context, def_buffer, reg);
    gen_register_enum_item(context, enum_buffer, reg);
  }
  return 0;
}

int32_t codegen_instruction(GContext *context, Instruction *instr) {
  const InstrForm *forms = Array_real_addr(instr->forms, 0);
  const uint32_t n_forms = Array_length(instr->forms);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_def);
  gen_instr_encoding_dec(context, dec_buffer, instr->name->ptr, forms, n_forms);
  gen_instr_encoding_def(context, def_buffer, instr->name->ptr, forms, n_forms);

  return 0;
}

codegen_t *get_codegen(uint32_t type) {
  switch (type) {
    case enum_Memory: {
      return (codegen_t *) codegen_memory;
    }
    case enum_Immediate: {
      return (codegen_t *) codegen_immediate;
    }
    case enum_RegisterGroup: {
      return (codegen_t *) codegen_register_group;
    }
    case enum_Instruction: {
      return (codegen_t *) codegen_instruction;
    }
  }
  return nullptr;
}
