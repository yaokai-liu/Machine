/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: define.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "char_t.h"
#include "context.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

const char_t MEM_DEC_FMT[] = "const Entry *MEM_%s(uint64_t base, uint64_t offset);\n";
void online_gen_memory_dec(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, MEM_DEC_FMT, mem->name->ptr);
  push_string(temp_buffer);
}

const char_t IMM_DEC_FMT[] = "const Entry *IMM_%s(uint64_t val);\n";
void online_gen_immediate_dec(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEC_FMT, imm->name->ptr);
  push_string(temp_buffer);
}

const char_t REG_DEC_FMT[] = "const Entry *const REG_%s;\n";
void online_gen_register_dec(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[256] = {};
  const char_t *name = reg->name->ptr;
  sprintf(temp_buffer, REG_DEC_FMT, name);
  push_string(temp_buffer);
}

const char_t MEM_DEF_FMT[] =
    "const Entry *MEM_%s(uint64_t base, uint64_t offset) {\n"
    "  Entry * entry = CURRENT_MACHINE->allocator->calloc(1, sizeof(Entry));\n"
    "  entry->type = enum_MEM_%s;\n"
    "  uint64_t number = 0;\n"
    "  number = setNumBits(number, %d, %d, base);\n"
    "  number = setNumBits(number, %d, %d, offset);\n"
    "  entry->value = number;\n"
    "  return entry;\n"
    "}\n";
void online_gen_memory_def(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(
      temp_buffer, MEM_DEF_FMT, mem->name->ptr, mem->name->ptr, mem->base->lower, mem->base->upper,
      mem->offset->lower, mem->offset->upper
  );
  push_string(temp_buffer);
}

const char_t IMM_DEF_FMT[] =
    "const Entry *IMM_%s(uint64_t val) {\n"
    "  Entry * entry = CURRENT_MACHINE->allocator->calloc(1, sizeof(Entry));\n"
    "  entry->type = enum_IMM_%s;\n"
    "  entry->value = val;\n"
    "  return entry;\n"
    "}\n";
void online_gen_immediate_def(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEF_FMT, imm->name->ptr, imm->name->ptr);
  push_string(temp_buffer);
}

const char_t REG_ENTRY_DEF_FMT[] = "const static Entry {\n"
                                   "  .type = enum_REG_%s,\n"
                                   "  .value = 0x%lx,\n"
                                   "} Entry_REG_%s;\n";
const char_t REG_DEF_FMT[] = "const Entry * const REG_%s = &Entry_REG_%s;\n";
void online_gen_register_def(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[256] = {};
  const char_t *name = reg->name->ptr;
  sprintf(temp_buffer, REG_ENTRY_DEF_FMT, name, reg->code, name);
  push_string(temp_buffer);
  sprintf(temp_buffer, REG_DEF_FMT, name, name);
  push_string(temp_buffer);
}

#define gen_type_sprintf(Type, var, ...)                           \
  do {                                                             \
    const uint32_t count = Array_length(context->var##Array);      \
    const Type *entries = Array_real_addr(context->var##Array, 0); \
    for (uint32_t i = 0; i < count; i++) {                         \
      sprintf(temp_buffer, __VA_ARGS__);                           \
      push_string(temp_buffer);                                    \
    }                                                              \
  } while (false)

#define gen_type_enum_item(Type, FMT, var) \
  gen_type_sprintf(Type, var, "  enum_" #FMT "_%s,\n", entries[i].name->ptr)

void gen_enum_item(GContext *context, const Machine *machine) {
  char_t temp_buffer[256] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_enum_record);
  sprintf(temp_buffer, "enum MACHINE_%s_ENTRY_ENUM {\n", machine->name->ptr);
  push_string(temp_buffer);
  gen_type_enum_item(Memory, MEM, mem);
  gen_type_enum_item(Immediate, IMM, imm);
  gen_type_enum_item(Register, REG, reg);
  gen_type_enum_item(RegisterGroup, GRP, grp);
  gen_type_enum_item(Set, SET, set);
  sprintf(temp_buffer, "  enum_MAX_ENUM_OF_%s\n};\n", machine->name->ptr);
  push_string(temp_buffer);
}

#define gen_mem_sprintf(...) gen_type_sprintf(Memory, mem, __VA_ARGS__)
#define gen_imm_sprintf(...) gen_type_sprintf(Immediate, imm, __VA_ARGS__)
#define gen_reg_sprintf(...) gen_type_sprintf(Register, reg, __VA_ARGS__)
void gen_context_dec(GContext *context) {
  char_t temp_buffer[512] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_declare);

  gen_mem_sprintf(MEM_DEC_FMT, entries[i].name->ptr);
  gen_imm_sprintf(IMM_DEC_FMT, entries[i].name->ptr);
  gen_reg_sprintf(REG_DEC_FMT, entries[i].name->ptr);
}

void gen_context_def(GContext *context) {
  char_t temp_buffer[1024] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_definition);

  gen_reg_sprintf(REG_ENTRY_DEF_FMT, entries[i].name->ptr, entries[i].code, entries[i].name->ptr);
  gen_mem_sprintf(
      MEM_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr, entries[i].base->lower,
      entries[i].base->upper, entries[i].offset->lower, entries[i].offset->upper
  );
  gen_imm_sprintf(IMM_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr);
  gen_reg_sprintf(REG_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr);
}

#define gen_grp_sprintf(...) gen_type_sprintf(RegisterGroup, grp, __VA_ARGS__)
#define gen_set_sprintf(...) gen_type_sprintf(Set, set, __VA_ARGS__)
void gen_context_set_grp_jump_table(GContext *) {
  // todo:
}
