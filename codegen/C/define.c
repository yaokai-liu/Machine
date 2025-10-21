/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: define.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "char_t.h"
#include "declare-fmt.h"
#include "generate.h"
#include "generated/tokens.gen.h"
#include "parse/context.h"
#include "trie-dump.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ctx_ident_real(ptr) ((char_t *) Array_virt2real(ident_array, ptr))

constexpr char_t MEM_DEF_HEAD_FMT[] =
    " {\n"
    "  Entry * entry = &CURRENT_MACHINE->entries[CURRENT_MACHINE->argCount];\n"
    "  entry->type = enum_MEM_%s;\n"
    "  entry->width = %lu;\n"
    "  uint64_t number = 0;\n";
constexpr char_t MEM_DEF_TAIL[] = "  entry->value = number;\n"
                                  "  CURRENT_MACHINE->argCount++;\n"
                                  "  return entry;\n"
                                  "}\n";
constexpr char_t IMM_DEF_FMT[] =
    "const Entry *IMM_%s(uint64_t val) {\n"
    "  Entry * entry = &CURRENT_MACHINE->entries[CURRENT_MACHINE->argCount];\n"
    "  entry->type = enum_IMM_%s;\n"
    "  entry->width = %lu;\n"
    "  entry->value = val;\n"
    "  CURRENT_MACHINE->argCount++;\n"
    "  return entry;\n"
    "}\n";
constexpr char_t REG_ENTRY_DEF_FMT[] = "static const Entry Entry_REG_%s = {\n"
                                       "  .type = enum_REG_%s, .width = %lu, .value = 0x%lx,\n"
                                       "};\n";
constexpr char_t REG_DEF_FMT[] = "const Entry * const REG_%s = &Entry_REG_%s;\n";

#define GenC_gen_type_fprintf(ostream, Type, var, ...)              \
  do {                                                              \
    const uint32_t count = Array_length(context->var##Array);       \
    const Type *entries = Array_real_addr(context->var##Array, 0);  \
    for (uint32_t i = 0; i < count; i++) {                          \
      fprintf(ostream, __VA_ARGS__);                                \
    }                                                               \
  } while (false)

#define GenC_gen_type_enum_item(ostream, Type, FMT, var) \
  GenC_gen_type_fprintf(ostream, Type, var, "  enum_" #FMT "_%s,\n", ctx_ident_real(entries[i].name))

static const char_t *type_string(const uint32_t id);

void GenC_gen_enum_item(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;
  fputs("enum ENTRY_TYPE_ENUM {\n", generator->ostream_lib);
  fputs("  enum_NONE,\n", generator->ostream_lib);
  GenC_gen_type_enum_item(generator->ostream_lib, Memory, MEM, mem);
  GenC_gen_type_enum_item(generator->ostream_lib, Immediate, IMM, imm);
  GenC_gen_type_enum_item(generator->ostream_lib, Register, REG, reg);
  fputs("  enum_BEGIN_SET_GRP,\n", generator->ostream_lib);
  GenC_gen_type_enum_item(generator->ostream_lib, RegisterGroup, GRP, grp);
  GenC_gen_type_enum_item(generator->ostream_lib, RecordSet, SET, set);
  fputs("  enum_TYPE_ENUM_UPPER_BOUND\n};\n", generator->ostream_lib);
}

inline const char_t *type_string(const uint32_t id) {
  switch (id) {
    case Machine_TOKEN_Immediate: return "IMM";
    case Machine_TOKEN_Memory: return "MEM";
    case Machine_TOKEN_Register: return "REG";
    case Machine_TOKEN_RegisterGroup: return "GRP";
    case Machine_TOKEN_RecordSet: return "SET";
    default: return nullptr;
  }
}

constexpr char_t ENTRY_TYPE_CHECK_FMT[] =
    "  if (!entry_type_check(enum_%s_%s, %s->type)) { return nullptr; }\n";
constexpr char_t ENTRY_TYPE_ADD_FMT[] = "  entry->subtypes[%u] = %s->type;\n";
constexpr char_t ENTRY_VALUE_SET_FMT[] = "  number = numSetBits(number, %d, %d, %s->value);\n";
constexpr char_t VALUE_SET_FMT[] = "  number = numSetBits(number, %d, %d, %s);\n";

extern void gen_mem_dec_fprintf(
    FILE *ostream, const Memory *mem, const ParseContext *context, const Array *ident_array
);

void GenC_gen_mem_def_fprintf(
    FILE *ostream, const Memory *mem, const ParseContext *context, const Array *ident_array
) {
  gen_mem_dec_fprintf(ostream, mem, context, ident_array);
  fprintf(ostream, MEM_DEF_HEAD_FMT, ctx_ident_real(mem->name), mem->width);
  const uint32_t n_items = Array_length(mem->items);
  const MemItem *items = Array_real_addr(mem->items, 0);
  for (uint32_t i = 0; i < n_items; i++) {
    if (items[i].type) {
      const Record *record = GContext_findRecord(context, items[i].type);
      if (record->typeid != Machine_TOKEN_Immediate) {
        const char_t *t_kind = type_string(record->typeid);
        const char_t *t_name = ctx_ident_real(items[i].type);
        fprintf(ostream, ENTRY_TYPE_CHECK_FMT, t_kind, t_name, ctx_ident_real(items[i].name));
        fprintf(ostream, ENTRY_TYPE_ADD_FMT, i, ctx_ident_real(items[i].name));
      }
    }
  }
  for (uint32_t i = 0; i < n_items; i++) {
    const char_t *fmt = items[i].type ? ENTRY_VALUE_SET_FMT: VALUE_SET_FMT;
    fprintf(
      ostream, fmt, items[i].start, items[i].start + items[i].width, ctx_ident_real(items[i].name)
    );
  }
  fputs(MEM_DEF_TAIL, ostream);
}

void GenC_gen_mem_def(FILE *ostream, const ParseContext *context, const Array *ident_array) {
  const uint32_t count = Array_length(context->memArray);
  const Memory *memories = Array_real_addr(context->memArray, 0);
  for (uint32_t i = 0; i < count; i++) {
    const Memory *mem = &memories[i];
    GenC_gen_mem_def_fprintf(ostream, mem, context, ident_array);
  }
}

constexpr char_t SET_GRP_VAL_TABLE_DEC_FMT[] = "static const enum ENTRY_TYPE_ENUM\n"
                                               "SET_GRP_VAL_TABLE[];\n";
constexpr char_t SET_GRP_STATE_TABLE_DEC_FMT[] = "static const struct set_grp_jump_state\n"
                                                 "SET_GRP_STATE_TABLE[];\n";
void GenC_gen_context_dec(const Generator *generator, const Machine *) {
  fputs(SET_GRP_VAL_TABLE_DEC_FMT, generator->ostream_lib);
  fputs(SET_GRP_STATE_TABLE_DEC_FMT, generator->ostream_lib);
}

#define gen_record_fprintf(ostream, Type, var, ...)                 \
  do {                                                              \
    const uint32_t count = Array_length(context->var##Array);       \
    const Type *entries = Array_real_addr(context->var##Array, 0);  \
    for (uint32_t i = 0; i < count; i++) {                          \
      fprintf(ostream, __VA_ARGS__);                                \
    }                                                               \
  } while (false)

#define gen_imm_fprintf(ostream, ...) gen_record_fprintf(ostream, Immediate, imm, __VA_ARGS__)
#define gen_reg_fprintf(ostream, ...) gen_record_fprintf(ostream, Register, reg, __VA_ARGS__)
void GenC_gen_context_def(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;

  fputs("static const Entry Entry_EOI = { .type = enum_NONE, .value = 0x0 };\n", generator->ostream_lib);
  gen_reg_fprintf(
    generator->ostream_lib, REG_ENTRY_DEF_FMT, ctx_ident_real(entries[i].name),
    ctx_ident_real(entries[i].name), entries[i].field.upper - entries[i].field.lower + 1,
    entries[i].code
  );

  fputs("const Entry *const EOI = &Entry_EOI;\n", generator->ostream_lib);
  GenC_gen_mem_def(generator->ostream_lib, context, ident_array);
  gen_imm_fprintf(
    generator->ostream_lib, IMM_DEF_FMT, ctx_ident_real(entries[i].name),
    ctx_ident_real(entries[i].name), entries[i].width
  );
  gen_reg_fprintf(
    generator->ostream_lib, REG_DEF_FMT, ctx_ident_real(entries[i].name),
    ctx_ident_real(entries[i].name), entries[i].field.upper - entries[i].field.lower + 1
  );
}

constexpr char_t SET_GRP_VAL_TABLE_HEAD_FMT[] = "static const enum ENTRY_TYPE_ENUM\n"
                                                "SET_GRP_VAL_TABLE[] = {\n";
constexpr char_t SET_GRP_STATE_TABLE_HEAD_FMT[] = "static const struct set_grp_jump_state\n"
                                                  "SET_GRP_STATE_TABLE[] = {\n";
#define val_case_item(ostream, Type, var, FMT)                                              \
  case Machine_TOKEN_##Type: {                                                              \
    const Type *var = Array_real_addr(context->var##Array, record->offset);                 \
    fprintf(ostream, "  [%u] = enum_" #FMT "_%s,\n", count + j, ctx_ident_real(var->name)); \
    break;                                                                                  \
  }
void GenC_gen_set_grp_jump_table(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;


  const uint32_t grp_count = Array_length(context->grpArray);
  const RegisterGroup *groups = Array_real_addr(context->grpArray, 0);
  const uint32_t set_count = Array_length(context->setArray);
  const RecordSet *sets = Array_real_addr(context->setArray, 0);
  uint32_t count = 0;

  count = 0;
  fputs(SET_GRP_VAL_TABLE_HEAD_FMT, generator->ostream_lib);
  for (uint32_t i = 0; i < grp_count; i++) {
    const uint32_t reg_count = Array_length(groups[i].registers);
    REFER(Register) *regs = Array_real_addr(groups[i].registers, 0);
    for (uint32_t j = 0; j < reg_count; j++) {
      const Register *reg = Array_virt2real(context->regArray, regs[j]);
      fprintf(generator->ostream_lib, "  [%u] = enum_REG_%s,\n", count + j, ctx_ident_real(reg->name));
    }
    count += reg_count;
  }
  for (uint32_t i = 0; i < set_count; i++) {
    const uint32_t item_count = Set_count(sets[i].items);
    const REFER(Identifier) *items = Set_data(sets[i].items);
    for (uint32_t j = 0; j < item_count; j++) {
      const Record *record = GContext_findRecord(context, items[j]);
      switch (record->typeid) {
        val_case_item(generator->ostream_lib, Memory, mem, MEM)
        val_case_item(generator->ostream_lib, Immediate, imm, IMM)
        val_case_item(generator->ostream_lib, Register, reg, REG)
        val_case_item(generator->ostream_lib, RegisterGroup, grp, GRP)
        val_case_item(generator->ostream_lib, RecordSet, set, SET)
        default: {
        }
      }
    }
    count += item_count;
  }
  fputs("};\n", generator->ostream_lib);

  count = 0;
  fputs(SET_GRP_STATE_TABLE_HEAD_FMT, generator->ostream_lib);
  for (uint32_t i = 0; i < grp_count; i++) {
    const uint32_t reg_count = Array_length(groups[i].registers);
    fprintf(generator->ostream_lib, "  [%u] = { .count=%u, .index=%u },\n", i, reg_count, count);
    count += reg_count;
  }
  for (uint32_t i = 0; i < set_count; i++) {
    const uint32_t item_count = Set_count(sets[i].items);
    fprintf(generator->ostream_lib, "  [%u] = { .count=%u, .index=%u },\n", grp_count + i, item_count, count);
    count += item_count;
  }
  fputs("};\n", generator->ostream_lib);
}

constexpr char_t REG_GRP_TABLE_HEAD[] = "static const enum ENTRY_TYPE_ENUM\n"
                                        "REG_GRP_VAL_TABLE[] = {\n";
void GenC_gen_reg_grp_table(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;
  fputs(REG_GRP_TABLE_HEAD, generator->ostream_lib);
  const Register *regs = Array_first_real(context->regArray);
  const uint32_t n_regs = Array_length(context->regArray);
  for (uint32_t i = 0; i < n_regs; i++) {
    const char_t *reg_name = Array_virt2real(ident_array, regs[i].name);
    const RegisterGroup *grp = Array_virt2real(context->grpArray, regs[i].group);
    const char_t *grp_name = Array_virt2real(ident_array, grp->name);
    fprintf(generator->ostream_lib, " [enum_REG_%s] = enum_GRP_%s,\n", reg_name, grp_name);
  }
  fputs("};\n", generator->ostream_lib);
}
