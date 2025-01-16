/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: define.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "char_t.h"
#include <stdint.h>

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
                             "} Entry_REG_%s;"
                             "const Entry * REG_%s = &Entry_REG_%s\n";
