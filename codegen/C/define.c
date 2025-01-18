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

const char_t MEM_ENUM_FMT[] = "enum_MEM_%s";
const char_t IMM_ENUM_FMT[] = "enum_IMM_%s";
const char_t REG_ENUM_FMT[] = "enum_REG_%s";
const char_t REG_DEC_FMT[] = "const Entry *REG_%s";
const char_t MEM_DEC_FMT[] = "Entry *MEM_%s(uint64_t base, uint64_t offset)";
const char_t IMM_DEC_FMT[] = "Entry *IMM_%s(uint64_t val)";

[[gnu::unused]]
const char_t STRUCT_MEM_FMT[] = "typedef struct {\n"
                                "  uint64_t offset : %d;\n"
                                "  uint64_t base : %d;\n"
                                "} struct_Memory_%s;\n";
[[gnu::unused]]
const char_t STRUCT_IMM_FMT[] = "typedef struct {\n"
                                "  uint64_t value : %d;\n"
                                "} struct_Immediate_%s;\n";

const char_t MEM_DEF_FMT[] =
    "{\n"
    "  Entry * entry = CURRENT_MACHINE->allocator->calloc(1, sizeof(Entry));\n"
    "  entry->type = enum_MEM_%s;\n"
    "  uint64_t number = 0;\n"
    "  number = setNumBits(number, %d, %d, base);\n"
    "  number = setNumBits(number, %d, %d, offset);\n"
    "  entry->value = number;\n"
    "  return entry;\n"
    "}\n";
const char_t IMM_DEF_FMT[] =
    "{\n"
    "  Entry * entry = CURRENT_MACHINE->allocator->calloc(1, sizeof(Entry));\n"
    "  entry->type = enum_IMM_%s;\n"
    "  entry->value = val;\n"
    "  return entry;\n"
    "}\n";
const char_t REG_DEF_FMT[] = "const static Entry {\n"
                             "  .type = enum_REG_%s,\n"
                             "  .value = enum_REG_%s,\n"
                             "} Entry_REG_%s;\n"
                             "const Entry * REG_%s = &Entry_REG_%s;\n";

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

void gen_memory_enum_item(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, MEM_ENUM_FMT, mem->name->ptr);
  push_string(temp_buffer);
  push_string(",\n");
}

void gen_memory_dec(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, MEM_DEC_FMT, mem->name->ptr);
  push_string(temp_buffer);
  push_string(";\n");
}

void gen_memory_def(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, MEM_DEC_FMT, mem->name->ptr);
  push_string(temp_buffer);
  sprintf(
      temp_buffer, MEM_DEF_FMT, mem->name->ptr, mem->base->lower, mem->base->upper,
      mem->offset->lower, mem->offset->upper
  );
  push_string(temp_buffer);
}

void gen_immediate_enum_item(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_ENUM_FMT, imm->name->ptr);
  push_string(temp_buffer);
  push_string(",\n");
}

void gen_immediate_dec(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEC_FMT, imm->name->ptr);
  push_string(temp_buffer);
  push_string(";\n");
}

void gen_immediate_def(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEC_FMT, imm->name->ptr);
  push_string(temp_buffer);
  sprintf(temp_buffer, IMM_DEF_FMT, imm->name->ptr);
  push_string(temp_buffer);
}

void gen_register_enum_item(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, REG_ENUM_FMT, reg->name->ptr);
  push_string(temp_buffer);
  push_string(",\n");
}

void gen_register_dec(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, REG_DEC_FMT, reg->name->ptr);
  push_string(temp_buffer);
  push_string(";\n");
}

void gen_register_def(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[512] = {};
  const char_t *name = reg->name->ptr;
  sprintf(temp_buffer, REG_DEF_FMT, name, name, name, name, name);
  push_string(temp_buffer);
}
