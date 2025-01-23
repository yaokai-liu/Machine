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
#include "pattern_match.h"
#include "target.h"
#include "tokens.gen.h"
#include <stdint.h>

int32_t online_codegen_memory(GContext *context, REFER(Memory) mem) {
  mem = Array_vert2real(context->memArray, mem);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_memory_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_memory_def);

  online_gen_memory_dec(context, dec_buffer, mem);
  online_gen_memory_def(context, def_buffer, mem);

  return 0;
}

int32_t online_codegen_immediate(GContext *context, REFER(Immediate) imm) {
  imm = Array_vert2real(context->immArray, imm);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_immediate_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_immediate_def);

  online_gen_immediate_dec(context, dec_buffer, imm);
  online_gen_immediate_def(context, def_buffer, imm);

  return 0;
}

int32_t online_codegen_register_group(GContext *context, REFER(RegisterGroup) grp) {
  grp = Array_vert2real(context->grpArray, grp);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_register_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_register_def);

  const uint32_t n_regs = Array_length(grp->registers);
  REFER(Register) *regs = (REFER(Register) *) Array_real_addr(grp->registers, 0);
  for (uint32_t i = 0; i < n_regs; i++) {
    const Register *reg = Array_vert2real(context->regArray, regs[i]);
    online_gen_register_dec(context, dec_buffer, reg);
    online_gen_register_def(context, def_buffer, reg);
  }
  return 0;
}

int32_t online_codegen_instruction(GContext *context, Instruction *instr) {
  instr = Array_vert2real(context->instrArray, instr);
  const InstrForm *forms = Array_real_addr(instr->forms, 0);
  const uint32_t n_forms = Array_length(instr->forms);

  Array *dec_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_dec);
  Array *def_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_def);

  online_gen_instr_encoding_dec(context, dec_buffer, instr->name->ptr, forms, n_forms);
  online_gen_instr_encoding_def(context, def_buffer, instr->name->ptr, forms, n_forms);

  return 0;
}

int32_t online_codegen_machine(GContext *context, Machine * /*unused*/) {
  gen_pattern_match(context, nullptr);
  gen_enum_item(context, nullptr);
  return 0;
}
int32_t codegen_machine(GContext *context, Machine *machine) {
  gen_context_dec(context);
  gen_context_def(context);
  gen_pattern_match(context, machine);
  gen_enum_item(context, machine);
  return 0;
}

codegen_t *get_online_codegen(uint32_t type) {
  switch (type) {
    case enum_Memory: {
      return (codegen_t *) online_codegen_memory;
    }
    case enum_Immediate: {
      return (codegen_t *) online_codegen_immediate;
    }
    case enum_RegisterGroup: {
      return (codegen_t *) online_codegen_register_group;
    }
    case enum_Instruction: {
      return (codegen_t *) online_codegen_instruction;
    }
    case enum_Machine: {
      return (codegen_t *) online_codegen_machine;
    }
    default: {
    }
  }
  return nullptr;
}

codegen_t *get_codegen(uint32_t type) {
  if (type == enum_Machine) { return (codegen_t *) codegen_machine; }
  return nullptr;
}
