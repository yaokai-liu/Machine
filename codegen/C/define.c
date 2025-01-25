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
#include "tokens.gen.h"
#include "trie-dump.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ctx_push_string(type, s) \
  do { Array_append(GContext_getOutputBuffer(context, CtxBuf_##type), s, strlen(s)); } while (false)

#define _push_string(buffer, s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

constexpr char_t MEM_DEC_FMT[] = "const Entry *MEM_%s(uint64_t base, uint64_t offset);\n";
void online_gen_memory_dec(GContext *, Array *buffer, const Memory *mem) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, MEM_DEC_FMT, mem->name->ptr);
  push_string(temp_buffer);
}

constexpr char_t IMM_DEC_FMT[] = "const Entry *IMM_%s(uint64_t val);\n";
void online_gen_immediate_dec(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEC_FMT, imm->name->ptr);
  push_string(temp_buffer);
}

constexpr char_t REG_DEC_FMT[] = "const Entry *const REG_%s;\n";
void online_gen_register_dec(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[256] = {};
  const char_t *name = reg->name->ptr;
  sprintf(temp_buffer, REG_DEC_FMT, name);
  push_string(temp_buffer);
}

constexpr char_t MEM_DEF_FMT[] =
    "const Entry *MEM_%s(uint64_t base, uint64_t offset) {\n"
    "  Entry * entry = &CURRENT_MACHINE->entries[CURRENT_MACHINE->argCount];\n"
    "  entry->type = enum_MEM_%s;\n"
    "  uint64_t number = 0;\n"
    "  number = numSetBits(number, %d, %d, base);\n"
    "  number = numSetBits(number, %d, %d, offset);\n"
    "  entry->value = number;\n"
    "  CURRENT_MACHINE->argCount++;\n"
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

constexpr char_t IMM_DEF_FMT[] =
    "const Entry *IMM_%s(uint64_t val) {\n"
    "  Entry * entry = &CURRENT_MACHINE->entries[CURRENT_MACHINE->argCount];\n"
    "  entry->type = enum_IMM_%s;\n"
    "  entry->value = val;\n"
    "  CURRENT_MACHINE->argCount++;\n"
    "  return entry;\n"
    "}\n";
void online_gen_immediate_def(GContext *, Array *buffer, const Immediate *imm) {
  char_t temp_buffer[512] = {};
  sprintf(temp_buffer, IMM_DEF_FMT, imm->name->ptr, imm->name->ptr);
  push_string(temp_buffer);
}

constexpr char_t REG_ENTRY_DEF_FMT[] = "const static Entry Entry_REG_%s = {\n"
                                       "  .type = enum_REG_%s,\n"
                                       "  .value = 0x%lx,\n"
                                       "};\n";
constexpr char_t REG_DEF_FMT[] = "const Entry * const REG_%s = &Entry_REG_%s;\n";
void online_gen_register_def(GContext *, Array *buffer, const Register *reg) {
  char_t temp_buffer[256] = {};
  const char_t *name = reg->name->ptr;
  sprintf(temp_buffer, REG_ENTRY_DEF_FMT, name, name, reg->code);
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

void gen_enum_item(GContext *context, const Machine *) {
  char_t temp_buffer[256] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_enums);
  push_string("enum ENTRY_TYPE_ENUM {\n");
  gen_type_enum_item(Memory, MEM, mem);
  gen_type_enum_item(Immediate, IMM, imm);
  gen_type_enum_item(Register, REG, reg);
  push_string("  enum_BEGIN_SET_GRP,\n");
  gen_type_enum_item(RegisterGroup, GRP, grp);
  gen_type_enum_item(Set, SET, set);
  push_string("  enum_TYPE_ENUM_UPPER_BOUND\n};\n");
}
constexpr char_t SET_GRP_VAL_TABLE_DEC_FMT[] = "const static enum ENTRY_TYPE_ENUM\n"
                                               "SET_GRP_VAL_TABLE[];\n";
constexpr char_t SET_GRP_STATE_TABLE_DEC_FMT[] = "const static struct set_grp_jump_state\n"
                                                 "SET_GRP_STATE_TABLE[];\n";
#define gen_mem_sprintf(...) gen_type_sprintf(Memory, mem, __VA_ARGS__)
#define gen_imm_sprintf(...) gen_type_sprintf(Immediate, imm, __VA_ARGS__)
#define gen_reg_sprintf(...) gen_type_sprintf(Register, reg, __VA_ARGS__)
void gen_context_dec(GContext *context) {
  char_t temp_buffer[512] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_exports);
  gen_mem_sprintf(MEM_DEC_FMT, entries[i].name->ptr);
  gen_imm_sprintf(IMM_DEC_FMT, entries[i].name->ptr);
  gen_reg_sprintf(REG_DEC_FMT, entries[i].name->ptr);

  ctx_push_string(declares, SET_GRP_VAL_TABLE_DEC_FMT);
  ctx_push_string(declares, SET_GRP_STATE_TABLE_DEC_FMT);
}

void gen_context_def(GContext *context) {
  char_t temp_buffer[1024] = {};
  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_definitions);

  gen_reg_sprintf(REG_ENTRY_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr, entries[i].code);
  gen_mem_sprintf(
      MEM_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr, entries[i].base->lower,
      entries[i].base->upper, entries[i].offset->lower, entries[i].offset->upper
  );
  gen_imm_sprintf(IMM_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr);
  gen_reg_sprintf(REG_DEF_FMT, entries[i].name->ptr, entries[i].name->ptr);
}

constexpr char_t SET_GRP_VAL_TABLE_HEAD_FMT[] = "const static enum ENTRY_TYPE_ENUM\n"
                                                "SET_GRP_VAL_TABLE[] = {\n";
constexpr char_t SET_GRP_STATE_TABLE_HEAD_FMT[] = "const static struct set_grp_jump_state\n"
                                                  "SET_GRP_STATE_TABLE[] = {\n";
#define val_case_item(Type, var, FMT)                                       \
  case enum_##Type: {                                                       \
    const Type *var = Array_real_addr(context->var##Array, record->offset); \
    sprintf(temp_buffer, "  enum_" #FMT "_%s,\n", var[i].name->ptr);        \
    _push_string(val_buffer, temp_buffer);                                  \
    break;                                                                  \
  }
#define gen_grp_sprintf(...) gen_type_sprintf(RegisterGroup, grp, __VA_ARGS__)
#define gen_set_sprintf(...) gen_type_sprintf(Set, set, __VA_ARGS__)
void gen_set_grp_jump_table(GContext *context, Machine *) {
  char_t temp_buffer[512] = {};
  Array *val_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  Array *sta_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));

  _push_string(val_buffer, SET_GRP_VAL_TABLE_HEAD_FMT);
  _push_string(sta_buffer, SET_GRP_STATE_TABLE_HEAD_FMT);

  uint32_t count = 0;
  const uint32_t grp_count = Array_length(context->grpArray);
  const RegisterGroup *groups = Array_real_addr(context->grpArray, 0);
  for (uint32_t i = 0; i < grp_count; i++) {
    const uint32_t reg_count = Array_length(groups[i].registers);
    REFER(Register) *regs = Array_real_addr(groups[i].registers, 0);
    for (uint32_t j = 0; j < reg_count; j++) {
      const Register *reg = Array_vert2real(context->regArray, regs[j]);
      sprintf(temp_buffer, "  enum_REG_%s,\n", reg->name->ptr);
      _push_string(val_buffer, temp_buffer);
    }
    sprintf(temp_buffer, "  { .count=%u, .index=%u },\n", reg_count, count);
    _push_string(sta_buffer, temp_buffer);
    count += reg_count;
  }

  const uint32_t set_count = Array_length(context->setArray);
  const Set *sets = Array_real_addr(context->setArray, 0);
  for (uint32_t i = 0; i < set_count; i++) {
    const uint32_t item_count = Array_length(sets[i].items);
    const SetItem *items = Array_real_addr(sets[i].items, 0);
    for (uint32_t j = 0; j < item_count; j++) {
      const Record *record = GContext_findRecord(context, items[j].name);
      switch (record->typeid) {
        val_case_item(Memory, mem, MEM);
        val_case_item(Immediate, imm, IMM);
        val_case_item(Register, reg, REG);
        val_case_item(RegisterGroup, grp, GRP);
        val_case_item(Set, set, SET);
        default: {
        }
      }
    }
    sprintf(temp_buffer, "  { .count=%u, .index=%u },\n", item_count, count);
    _push_string(sta_buffer, temp_buffer);
    count += item_count;
  }
  _push_string(val_buffer, "};\n");
  _push_string(sta_buffer, "};\n");

  Array *buffer = GContext_getOutputBuffer(context, CtxBuf_definitions);
  Array_concat(buffer, val_buffer);
  Array_concat(buffer, sta_buffer);
  releasePrimeArray(val_buffer);
  releasePrimeArray(sta_buffer);
}
