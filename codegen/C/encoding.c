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
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "encoding.h"
#include "array.h"
#include "char_t.h"
#include "enum.h"
#include "generated/tokens.gen.h"
#include "parse/context.h"
#include "stdint.h"
#include "terminal.h"
#include "trie-dump.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

constexpr char_t ENCODING_DEF_FMT_HEAD[] = "{\n"
                                           "  uint32_t size = 0;\n"
                                           "  uint32_t index = 0;\n"
                                           "  uint64_t value = 0;\n"
                                           "  uint8_t bytes[%u] = {};\n";

constexpr char_t ENCODING_DEF_FMT_TAIL[] = "  Array_append(buffer, bytes, size);\n"
                                           "  return size;\n"
                                           "}\n";

constexpr char_t ENCODING_DEC_FMT[] =
    "uint32_t encoding_%s_%u(Array *buffer, const Entry *entries[])";
constexpr char_t ENCODING_DEC_NO_ARGS_FMT[] =
    "uint32_t encoding_%s_%u(Array *buffer, const Entry *[])";
constexpr char_t ENCODING_NAME_FMT[[maybe_unused]][] = "encoding_%s_%u";

#define ctx_ident_real(ptr) ((char_t *) Array_virt2real(ident_array, ptr))

void instr_pattern_to_str(
    const ParseContext *, const Array *ident_array, const char_t *instr_op, const InstrForm *form,
    FILE *ostream
) {
  fprintf(ostream, "// %s(", instr_op);
  if (!form->pattern->args) {
    fputs(")\n", ostream);
    return ;
  }
  const uint32_t count = Array_length(form->pattern->args);
  const Parameter *params = Array_real_addr(form->pattern->args, 0);
  for (uint32_t i = 0; i < count; i++) {
    const Identifier *type = Array_virt2real(ident_array, params[i].type);
    const Identifier *name = Array_virt2real(ident_array, params[i].name);
    fprintf(ostream, "%s %s", type, name);
    fputs((i < count - 1) ? ", " : ")\n", ostream);
  }
}

void GenC_gen_instr_encoding_dec(
    const ParseContext *, const Array *, const char_t *instr_op, const InstrForm[],
    uint32_t n_forms, FILE *ostream
) {
  for (uint32_t i = 0; i < n_forms; ++i) {
    fprintf(ostream, ENCODING_DEC_FMT, instr_op, i);
    fputs(";\n", ostream);
  }
}

void GenC_gen_instr_encoding_def(
    const ParseContext *context, const Array *ident_array, const char_t *instr_op,
    const InstrForm forms[], uint32_t n_forms, FILE *ostream
) {
  for (uint32_t i = 0; i < n_forms; ++i) {
    instr_pattern_to_str(context, ident_array, instr_op, &forms[i], ostream);
    const char_t * const encoding_dec_fmt =
        forms[i].pattern->used_args ? ENCODING_DEC_FMT : ENCODING_DEC_NO_ARGS_FMT;
    fprintf(ostream, encoding_dec_fmt, instr_op, i);
    const uint32_t n_bytes = forms[i].width / 8;
    fprintf(ostream, ENCODING_DEF_FMT_HEAD, n_bytes);
    const PatternArgs *arg_array = forms[i].pattern->args;
    if (arg_array) {
      const uint32_t n_args = Array_length(arg_array);
      const Parameter *args = Array_real_addr(arg_array, 0);
      for (uint32_t j = 0; j < n_args; j++) {
        if (!args[j].used) { continue; }
        fprintf(ostream, "  const Entry *%s = entries[%u];\n", ctx_ident_real(args[j].name), j);
      }
    }
    GenC_gen_instr_form(context, ident_array, &forms[i], ostream);
    fputs(ENCODING_DEF_FMT_TAIL, ostream);
  }
}

constexpr char_t INSTR_EXEC_DEC[] = "uint32_t encodingInstr(Array *buffer, uint32_t entry_offset, ...);\n";
void GenC_gen_instr_dec(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;

  const uint32_t n_instr = Array_length(context->instrArray);
  const Instruction *instructions = Array_real_addr(context->instrArray, 0);
  for (uint32_t i = 0; i < n_instr; i++) {
    const uint32_t n_forms = Array_length(instructions[i].forms);
    const InstrForm *forms = Array_real_addr(instructions[i].forms, 0);
    GenC_gen_instr_encoding_dec(
        context, ident_array, ctx_ident_real(instructions[i].name), forms, n_forms, generator->ostream_lib
    );
  }
  fputs(INSTR_EXEC_DEC, generator->ostream_lib);
}

constexpr char_t INSTR_EXEC_DEF[] =
    "uint32_t encodingInstr(Array *buffer, uint32_t entry_offset, ...) {\n"
    "  const Entry *entries[MAX_ARGS] = {};\n"
    "  va_list args;\n"
    "  va_start(args, entry_offset);\n"
    "  uint32_t n_args = 0;\n"
    "  for (; n_args < MAX_ARGS; n_args ++) {\n"
    "    Entry * entry = va_arg(args, Entry *);\n"
    "    if (!entry) { return 0; }\n"
    "    if (entry->type == enum_NONE) { break; }\n"
    "    entries[n_args] = entry;\n"
    "  }\n"
    "  va_end(args);\n"
    "  CURRENT_MACHINE->argCount = 0;\n"
    "  return convert_instr_to_bytes(entry_offset, buffer, entries, n_args);\n"
    "}\n";
void GenC_gen_instr_def(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;

  const uint32_t n_instr = Array_length(context->instrArray);
  const Instruction *instructions = Array_real_addr(context->instrArray, 0);
  for (uint32_t i = 0; i < n_instr; i++) {
    const uint32_t n_forms = Array_length(instructions[i].forms);
    const InstrForm *forms = Array_real_addr(instructions[i].forms, 0);
    GenC_gen_instr_encoding_def(
        context, ident_array, ctx_ident_real(instructions[i].name), forms, n_forms, generator->ostream_lib
    );
  }
  fputs(INSTR_EXEC_DEF, generator->ostream_lib);
}

constexpr char_t JUMP_KEY_DEC_FMT[] = "static const struct jump_item\n"
                                      "JUMP_KEY_TABLE[];\n";
constexpr char_t JUMP_STATE_DEC_FMT[] = "static const struct jump_state\n"
                                        "JUMP_STATE_TABLE[];\n";
void GenC_gen_jump_table_dec(const Generator *generator, const Machine *) {
  fputs(JUMP_KEY_DEC_FMT, generator->ostream_lib);
  fputs(JUMP_STATE_DEC_FMT, generator->ostream_lib);
}
constexpr char_t JUMP_KEY_HEADER_FMT[] = "static const struct jump_item\n"
                                         "JUMP_KEY_TABLE[] = {\n";
constexpr char_t JUMP_STATE_HEADER_FMT[] = "static const struct jump_state\n"
                                           "JUMP_STATE_TABLE[] = {\n";
constexpr char_t KEY_ITEM_FMT[] = "  [%u] = { .expected_type = enum_%s_%s, .next_state_index = %lu },\n";
constexpr char_t STATE_ITEM_FMT[] = "  [%u] = { .count = %u, .index = %u, .fn_encoding = %s },\n";
constexpr char_t STATE_ITEM_ENCODING_FMT[] = "  [%u] = { .count = %u, .index = %u, .fn_encoding = encoding_%s_%u },\n";
#define val_case_item(ostream, Type, var, PREFIX)                                                 \
  case Machine_TOKEN_##Type: {                                                                    \
    const Type *var = Array_real_addr(context->var##Array, record->offset);                       \
    fprintf(ostream, KEY_ITEM_FMT, i, PREFIX, ctx_ident_real(var->name), key_items[i].next_node); \
    break;                                                                                        \
  }
void GenC_gen_jump_table_def(const Generator *generator, const Machine *machine) {
  const ParseContext *context = machine->context;
  const Array *ident_array = generator->ident_array;

  fputs(JUMP_KEY_HEADER_FMT, generator->ostream_lib);
  const uint32_t key_count = Array_length(context->keyArray);
  const TrieKeyItem *key_items = Array_real_addr(context->keyArray, 0);
  for (uint32_t i = 0; i < key_count; i++) {
    const Record *record = Array_virt2real(context->recordArray, (REFER(Record)) key_items[i].key);
    switch (record->typeid) {
      val_case_item(generator->ostream_lib, Memory, mem, "MEM")
      val_case_item(generator->ostream_lib, Immediate, imm, "IMM")
      val_case_item(generator->ostream_lib, Register, reg, "REG")
      val_case_item(generator->ostream_lib, RegisterGroup, grp, "GRP")
      val_case_item(generator->ostream_lib, RecordSet, set, "SET")
      default: {
      }
    }
  }
  fputs("};\n", generator->ostream_lib);

  fputs(JUMP_STATE_HEADER_FMT, generator->ostream_lib);
  const uint32_t state_count = Array_length(context->stateArray);
  const TrieNodeItem *state_items = Array_real_addr(context->stateArray, 0);
  for (uint32_t i = 0; i < state_count; i++) {
    const uint32_t instr_ndx = (uint32_t) ((uint64_t) state_items[i].value >> 32);
    const Instruction *instr = GContext_getInstruction(context, instr_ndx);
    const uint32_t form_ndx = (uint32_t) (uint64_t) state_items[i].value;
    if (form_ndx) {
      fprintf(
        generator->ostream_lib, STATE_ITEM_ENCODING_FMT, i, state_items[i].count,
        state_items[i].offset, ctx_ident_real(instr->name), form_ndx - 1
      );
    } else {
      fprintf(
        generator->ostream_lib, STATE_ITEM_FMT, i, state_items[i].count,
        state_items[i].offset, "nullptr");
    }
  }
  fputs("};\n", generator->ostream_lib);
}

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define getDefaultMappingBit(default_bit)                      \
  do {                                                         \
    if (items->default_eval) {                                 \
      default_bit = ((uint64_t) items->default_eval->lhs) & 1; \
      default_bit = default_bit ? -1 : 0;                      \
    }                                                          \
  } while (false)

#define getMappingItem(_items, p_bf)                                                       \
  Array_real_addr(                                                                         \
      (_items)->itemArray,                                                                 \
      ((uint32_t) (uint64_t) AVLTree_get((_items)->itemTree, BitField_toUint64(p_bf)) - 1) \
  )

#define MAX_IDENT_LEN 64
int32_t
    eval_to_val(
    const ParseContext *, const Array * const ident_array, const Evaluable *evaluable,
    const Pattern *, FILE *ostream
) {
  if (Machine_TOKEN_NUMBER == evaluable->type) {
    const uint64_t number = (uint64_t) evaluable->lhs;
    return fprintf(ostream, "0x%llX", number);
  }
  Variable *variable = (Variable *) evaluable->lhs;
  const Identifier *ident = variable->lhs;

  switch (evaluable->type) {
    case Machine_TOKEN_OP_WIDTH: {
      switch (variable->type) {
        case Machine_TOKEN_IDENTIFIER: {
          return fprintf(ostream, "%s->width", ctx_ident_real(ident));
        }
        case Machine_TOKEN_MemItem: {
          const MemItem *item = variable->rhs;
          //    if (item->type) {
          //      uint32_t width = item->width;
          //      return fprintf(ostream, "%s->subtypes[%u]->width", ctx_ident_real(ident),
          //      item->index);
          //    }
          const uint32_t width = item->width;
          return fprintf(ostream, "%u", width);
        }
        default:;
      }
      break;
    }
    case Machine_TOKEN_BIT_FIELD: {
      BitField p_bf = BitField_fromUint64((uint64_t) evaluable->rhs);
      uint32_t width = p_bf.upper - p_bf.lower + 1;
      // TODO: if record refers to a set there may has different behaviors, please solve it.
      if (width == 0) { return fprintf(ostream, "0"); }
      if (p_bf.lower == 0) {
        return fprintf(ostream, "((%s->value)&UINT_N_MAX(%d))", ctx_ident_real(ident), width);
      }
      return fprintf(
          ostream, "((%s->value>>%d)&UINT_N_MAX(%d))", ctx_ident_real(ident), p_bf.lower, width
      );
    }
    case Machine_TOKEN_Variable: {
      switch (variable->type) {
        case Machine_TOKEN_IDENTIFIER: {
          return fprintf(ostream, "%s->value", ctx_ident_real(ident));
        }
        case Machine_TOKEN_MemItem: {
          const MemItem *item = variable->rhs;
          uint32_t width = item->width;
          if (width == 0) { return fprintf(ostream, "0"); }
          if (item->start == 0) {
            return fprintf(ostream, "((%s->value)&UINT_N_MAX(%d))", ctx_ident_real(ident), width);
          }
          return fprintf(
              ostream, "((%s->value>>%d)&UINT_N_MAX(%d))", ctx_ident_real(ident), item->start, width
          );
        }
        default:;
      }
    }
    default:;
  }
  return -1;
}

constexpr char_t TYPE_ENUM_FMT[] = "enum_%s_%s";
#define findParameterNdxAndType(ident)                         \
  do {                                                         \
    const uint32_t length = Array_length(pattern->args);       \
    const Parameter *params = Array_first_real(pattern->args); \
    for (uint32_t i = 0; i < length; i++) {                    \
      if (params[i].name == ident) {                           \
        type = params[i].type;                                 \
        break;                                                 \
      }                                                        \
    }                                                          \
  } while (false)
#define type_case_item(Type, var, PREFIX)                                   \
  case Machine_TOKEN_##Type: {                                              \
    const Type *var = Array_real_addr(context->var##Array, record->offset); \
    fprintf(ostream, TYPE_ENUM_FMT, PREFIX, ctx_ident_real(var->name));     \
    break;                                                                  \
  }
void type_to_val(
    const ParseContext *context, const Array *ident_array, const Identifier *ident,
    const Pattern *, FILE *ostream
) {
  const Record *record = GContext_findRecord(context, ident);
  switch (record->typeid) {
    type_case_item(Memory, mem, "MEM")
    type_case_item(Immediate, imm, "IMM")
    type_case_item(Register, reg, "REG")
    type_case_item(RegisterGroup, grp, "GRP")
    type_case_item(RecordSet, set, "SET")
    default:;
  }
}

#define bin_op_case_item(op, fmt)                       \
  case op: {                                            \
    fprintf(ostream, fmt, temp_buffer1, temp_buffer2);  \
    break;                                              \
  }
#define sin_op_case_item(op, fmt)         \
  case op: {                              \
    fprintf(ostream, fmt, temp_buffer2);  \
    break;                                \
  }
struct OpDecorator {
  const char_t *pre;
  const char_t *mid;
  const char_t *suf;
};
int32_t expr_to_val(
    const ParseContext *context, const Array *ident_array, const Expr *expr, const Pattern *pattern,
    FILE *ostream
) {
  if (expr->type == AS_ID) {
    eval_to_val(context, ident_array, expr->rhs, pattern, ostream);
    return 0;
  }
  if (expr->type == CB_IN) {
    const Variable *var = (Variable *) expr->lhs;
    const Identifier *supper_type = expr->rhs;
    const Identifier *ident = var->lhs;
    fputs("entry_type_check(", ostream);
    type_to_val(context, ident_array, supper_type, pattern, ostream);
    fputs(", ", ostream);
    if (var->type == Machine_TOKEN_IDENTIFIER) {
      fprintf(ostream, "%s->type)", ctx_ident_real(ident));
    } else if (var->type == Machine_TOKEN_MemItem) {
      const MemItem *item = (MemItem *) var->rhs;
      fprintf(ostream, "%s->subtypes[%u])", ctx_ident_real(ident), item->index);
    }
    return 0;
  }

  if (expr->type >= RECURSIVE_OP_MAX) { return -1; }

  struct OpDecorator decorator = {};
  switch (expr->type) {
    case Machine_TOKEN_BOOL_OR: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "||", .suf = "" }; break;
    }
    case Machine_TOKEN_BOOL_AND: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "&&", .suf = "" }; break;
    }
    case Machine_TOKEN_BOOL_NOT: {
      decorator = (struct OpDecorator) { .pre = "!", .mid = "", .suf = "" }; break;
    }
    case CB_LT: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "<", .suf = "" }; break;
    }
    case CB_LE: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "<=", .suf = "" }; break;
    }
    case CB_GT: {
      decorator = (struct OpDecorator) { .pre = "", .mid = ">", .suf = "" }; break;
    }
    case CB_GE: {
      decorator = (struct OpDecorator) { .pre = "", .mid = ">=", .suf = "" }; break;
    }
    case CB_EQ: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "==", .suf = "" }; break;
    }
    case CB_NE: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "!=", .suf = "" }; break;
    }
    case AS_INV: {
      decorator = (struct OpDecorator) { .pre = "~", .mid = "", .suf = "" }; break;
    }
    case AB_OR: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "|", .suf = "" }; break;
    }
    case AB_AND: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "&", .suf = "" }; break;
    }
    case AB_XOR: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "^", .suf = "" }; break;
    }
    case AB_ADD: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "+", .suf = "" }; break;
    }
    case AB_SUB: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "-", .suf = "" }; break;
    }
    case AB_MUL: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "*", .suf = "" }; break;
    }
    case AB_DIV: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "/", .suf = "" }; break;
    }
    case AB_MOD: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "%", .suf = "" }; break;
    }
    case AB_LSH: {
      decorator = (struct OpDecorator) { .pre = "", .mid = "<<", .suf = "" }; break;
    }
    case AB_RSH: {
      decorator = (struct OpDecorator) { .pre = "", .mid = ">>", .suf = "" }; break;
    }
    default: {
    }
  }
  fprintf(ostream, "(%s", decorator.pre);
  if (expr->lhs) {
    expr_to_val(context, ident_array, expr->lhs, pattern, ostream);
  }
  fputs(decorator.mid, ostream);
  if (expr->rhs) {
    expr_to_val(context, ident_array, expr->rhs, pattern, ostream);
  }
  fprintf(ostream, "%s)", decorator.suf);
  return 0;
}

// TODO: GenC_gen_mapping_item is in a recursive call chain,
//  maybe it will cause an out of memory, please solve it.
void GenC_gen_mapping_item(
    const ParseContext *context, const Array * const ident_array, MappingItems *items,
    const BitField *bit_field, const Pattern *pattern, FILE *ostream
) {
  MappingItem *item = getMappingItem(items, bit_field);
  uint64_t default_bit = 0;
  getDefaultMappingBit(default_bit);
  if (!item) {
    fprintf(
        ostream, "  value = numSetBits(value, %d, %d, %lld);\n", bit_field->lower,
        bit_field->upper + 1, (int64_t) default_bit
    );
    return ;
  }

  const uint32_t bu = item->field.upper;
  const uint32_t bl = item->field.lower;

  if (bl > bit_field->lower) {
    const BitField lower_bf = {.lower = bit_field->lower, .upper = bl - 1};
    GenC_gen_mapping_item(context, ident_array, items, &lower_bf, pattern, ostream);
  }
  if (item->type == Machine_TOKEN_Arith_0_Expr) {
    fprintf(ostream, "    value = numSetBits(value, %d, %d, ", bl, bu + 1);
    expr_to_val(context, ident_array, item->target, pattern, ostream);
    fputs(");\n", ostream);
  } else if (item->type == Machine_TOKEN_Switchable) {
    const Switchable *switchable = item->target;
    BitField bf = {.lower = bl, .upper = bu};
    GenC_gen_switchable(context, ident_array, switchable, &bf, pattern, ostream);
  }
  if (bu < bit_field->upper) {
    const BitField upper_bf = {.lower = bu + 1, .upper = bit_field->upper};
    GenC_gen_mapping_item(context, ident_array, items, &upper_bf, pattern, ostream);
  }
}


#define setEncodingNumber(expr) do {                          \
  fputs("    setEncodingNumber(", ostream);                   \
  expr_to_val(context, ident_array, expr, pattern, ostream);  \
  fprintf(ostream, ");\n", count);                            \
} while (false)

#define pushEncodingNumberExpr(expr, count) do {              \
  fputs("    pushEncodingNumber(", ostream);                  \
  expr_to_val(context, ident_array, expr, pattern, ostream);  \
  fprintf(ostream, ", %u);\n", count);                        \
} while (false)

#define numberToInstrBytes(number, count) \
  fprintf(ostream, "    pushEncodingNumber(0x%lX, %u);\n", number, count)

#define push_expr(expr, p_bf)                                                 \
  do {                                                                        \
    if (((uint64_t) p_bf) > 64) {                                             \
      uint32_t bl = (p_bf)->lower;                                            \
      uint32_t bu = (p_bf)->upper;                                            \
      fprintf(ostream, "    value = numSetBits(value, %d, %d, ", bl, bu + 1); \
      expr_to_val(context, ident_array, expr, pattern, ostream);              \
      fputs(");\n", ostream);                                                 \
    } else {                                                                  \
      fputs("    pushEncodingNumber(", ostream);                              \
      expr_to_val(context, ident_array, expr, pattern, ostream);              \
      fprintf(ostream, ", %u);\n", ((uint32_t) (uint64_t) p_bf) / 8);         \
    }                                                                         \
  } while (false)

void GenC_gen_switchable(
    const ParseContext *context, const Array * const ident_array, const Switchable *switchable,
    BitField *p_bf, const Pattern *pattern, FILE *ostream
) {
  const Options *options = switchable->options;
  const Arith_0_Expr *exprs = Array_first_real(switchable->options);
  fputs("    if (", ostream);
  expr_to_val(context, ident_array, switchable->expr, pattern, ostream);
  fputs(") {\n  ", ostream);
  push_expr(&exprs[0], p_bf);
  if (Array_length(options) > 1) {
    fputs("    } else {\n  ", ostream);
    push_expr(&exprs[1], p_bf);
  }
  fputs("    }\n", ostream);
}

void GenC_gen_layout(
    const ParseContext *context, const Array *ident_array, const Layout *layout, uint32_t width,
    const Pattern *pattern, FILE *ostream
) {
  switch (layout->type) {
    case Machine_TOKEN_Arith_0_Expr: {
      const Expr *expr = layout->target;
      if (width == (uint32_t) -1) {
        const Evaluable *eval = expr->rhs;
        switch (eval->type) {
          case Machine_TOKEN_OP_WIDTH: {
            pushEncodingNumberExpr(expr, 1);
            fputs("    size += 1;\n", ostream);
            break;
          }
          case Machine_TOKEN_BIT_FIELD: {
            const BitField bf = BitField_fromUint64((uint64_t) eval->rhs);
            width = ((bf.upper - bf.lower) / 8) + 1;
            pushEncodingNumberExpr(expr, width);
            fprintf(ostream, "    size += %u;\n", width / 8);
            break;
          }
          case Machine_TOKEN_NUMBER: {
            width = 0;
            uint64_t num = (uint64_t) eval->lhs;
            while (num) {
              num >>= 3;
              width++;
            }
            pushEncodingNumberExpr(expr, width);
            fprintf(ostream, "    size += %u;\n", width / 8);
            break;
          }
          case Machine_TOKEN_Variable: {
            const Evaluable width_eval = {
                .type = Machine_TOKEN_OP_WIDTH, .lhs = eval->lhs, .rhs = nullptr
            };
            fputs("    pushEncodingNumber(", ostream);
            expr_to_val(context, ident_array, expr, pattern, ostream);
            fputs(", ", ostream);
            eval_to_val(context, ident_array, &width_eval, pattern, ostream);
            fputs(" / 8);\n", ostream);
            fputs("    size += ", ostream);
            eval_to_val(context, ident_array, &width_eval, pattern, ostream);
            fputs(" / 8;\n", ostream);
            break;
          }
          default:;
        }
      } else {
        pushEncodingNumberExpr(expr, width / 8);
        fprintf(ostream, "    size += %u;\n", width / 8);
      }
      break;
    }
    case Machine_TOKEN_MappingItems: {
      MappingItems *items = layout->target;
      for (uint32_t i = 0; i < width; i += 64) {
        fputs("    value = 0;\n", ostream);
        BitField bf = {.lower = i, .upper = min(i + 63, width - 1)};
        GenC_gen_mapping_item(context, ident_array, items, &bf, pattern, ostream);
        fprintf(ostream, "    pushInstrBytes(%d);\n", min(64, width - i) / 8);
      }
      fprintf(ostream, "    size += %u;\n", width / 8);
      break;
    }
    case Machine_TOKEN_Switchable: {
      GenC_gen_switchable(
        context, ident_array, layout->target, (void *) (uint64_t) width,
        pattern, ostream
      );
      fprintf(ostream, "    size += %u;\n", width / 8);
      break;
    }
    default:;
  }
}

constexpr char_t FORM_CHECK_FAULT[] = "{\n"
                                      "    CURRENT_MACHINE->err_type = ERR_FORM_CHECK_FAULT;\n"
                                      "    CURRENT_MACHINE->err_info[0] = -1;\n"
                                      "    CURRENT_MACHINE->err_info[1] = -1;\n"
                                      "    return 0;\n"
                                      "  }\n";

void GenC_gen_form_check(
    const ParseContext *context, const Array *ident_array, const InstrForm *form, FILE *ostream
) {
  FormCheck *check = form->check;
  fputs("  /* __FORM_CHECK__ */\n  if (!", ostream);
  expr_to_val(context, ident_array, check->expr, form->pattern, ostream);
  fputs(") ", ostream);
  fputs(FORM_CHECK_FAULT, ostream);
}

int32_t GenC_gen_instr_part(
    const ParseContext *context, const Array *ident_array, const InstrForm *form,
    const InstrPart *part, FILE *ostream
) {
  const Identifier *name = part->name;
  const uint32_t width = part->width;
  const Layout *layout = part->layout;
  const Condition *condition = part->condition;
  if (condition) {
    fprintf(ostream, "  /* %s */\n  if (", ctx_ident_real(name));
    expr_to_val(context, ident_array, condition->expr, form->pattern, ostream);
    fputs(") {\n", ostream);
  } else {
    fprintf(ostream, "  /* %s */ {\n", ctx_ident_real(name));
  }
  GenC_gen_layout(context, ident_array, layout, width, form->pattern, ostream);
  fputs("  }\n", ostream);
  return 0;
}

int32_t GenC_gen_instr_form(
    const ParseContext *context, const Array *ident_array, const InstrForm *form, FILE *ostream
) {
  const uint32_t n_parts = Array_length(form->parts);
  const InstrPart * const parts = Array_real_addr(form->parts, 0);
  if (form->check) { GenC_gen_form_check(context, ident_array, form, ostream); }
  for (uint32_t i = 0; i < n_parts; i++) {
    GenC_gen_instr_part(context, ident_array, form, &parts[i], ostream);
  }
  return 0;
}
