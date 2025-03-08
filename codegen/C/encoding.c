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
#include "context.h"
#include "tokens.gen.h"
#include "stdint.h"
#include "terminal.h"
#include "trie-dump.h"
#include <stdio.h>
#include <string.h>

constexpr char_t ENCODING_DEF_FMT_HEAD[] = "{\n"
                                           "  uint32_t size = 0;\n"
                                           "  uint8_t bytes[%u] = {};\n"
                                           "  uint64_t value = 0;\n"
                                           "  uint32_t index = 0;\n";

constexpr char_t ENCODING_DEF_FMT_TAIL[] = "  Array_append(buffer, bytes, size);\n"
                                           "  return size;\n"
                                           "}\n";

constexpr char_t ENCODING_DEC_FMT[] =
    "uint32_t encoding_%s_%u(Array *buffer, const Entry *entries[])";
constexpr char_t ENCODING_DEC_NO_ARGS_FMT[] =
    "uint32_t encoding_%s_%u(Array *buffer, const Entry *[])";
constexpr char_t ENCODING_NAME_FMT[] = "encoding_%s_%u";

#define ctx_push_string(type, s)                                                     \
  do {                                                                               \
    Array_append(Generator_getOutputBuffer(generator, GenBuf_##type), s, strlen(s)); \
  } while (false)

#define _push_string(buffer, s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

int32_t online_gen_instr_encoding_dec(
    const GContext *, Array *buffer, const char_t *instr_op, const InstrForm[], uint32_t n_forms
) {
  char_t head_buffer[sizeof(ENCODING_DEF_FMT_HEAD) + 256];
  for (uint32_t i = 0; i < n_forms; ++i) {
    sprintf(head_buffer, ENCODING_DEC_FMT, instr_op, i);
    push_string(head_buffer);
    push_string(";\n");
  }
  return 0;
}

int32_t online_gen_instr_encoding_def(
    const GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
) {
  char_t head_buffer[sizeof(ENCODING_DEF_FMT_HEAD) + 256];
  for (uint32_t i = 0; i < n_forms; ++i) {
    const PatternArgs *arg_array = forms[i].pattern->args;
    const char_t * const encoding_dec_fmt =
        arg_array ? ENCODING_DEC_FMT : ENCODING_DEC_NO_ARGS_FMT;
    sprintf(head_buffer, encoding_dec_fmt, instr_op, i);
    push_string(head_buffer);
    const uint32_t n_bytes = forms[i].width / 8;
    sprintf(head_buffer, ENCODING_DEF_FMT_HEAD, n_bytes);
    push_string(head_buffer);
    if (arg_array) {
      const uint32_t n_args = Array_length(arg_array);
      const Parameter *args = Array_real_addr(arg_array, 0);
      for (uint32_t j = 0; j < n_args; j++) {
        if (!args[j].used) { continue; }
        sprintf(head_buffer, "  const Entry *%s = entries[%u];\n", args[j].name->ptr, j);
        push_string(head_buffer);
      }
    }
    codegen_instr_form(context, buffer, &forms[i]);
    push_string(ENCODING_DEF_FMT_TAIL);
  }
  return 0;
}

const char_t INSTR_EXEC_DEC_FMT[] = "uint32_t %s(Array *buffer, ...);\n";
const char_t INSTR_EXEC_DEF_HEAD_FMT[] = "uint32_t %s(Array *buffer, ...) {\n"
                                         "  constexpr uint32_t entry_offset = %u;\n";
const char_t INSTR_EXEC_DEF_BODY[] = "  instrExecDefPrincipalPart();\n}\n";

void gen_instr_exec(Generator *generator, const Machine *machine) {
  char_t temp_buffer[512] = {};
  const GContext *context = machine->context;
  Array *dec_buffer = Generator_getOutputBuffer(generator, GenBuf_declares);
  Array *def_buffer = Generator_getOutputBuffer(generator, GenBuf_definitions);
  Array *encoding_dec_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  Array *encoding_def_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));

  const uint32_t n_instr = Array_length(context->instrArray);
  const Instruction *instructions = Array_real_addr(context->instrArray, 0);
  for (uint32_t i = 0; i < n_instr; i++) {
    const uint32_t n_forms = Array_length(instructions[i].forms);
    const InstrForm *forms = Array_real_addr(instructions[i].forms, 0);
    online_gen_instr_encoding_dec(
        context, encoding_dec_buffer, instructions[i].name->ptr, forms, n_forms
    );
    online_gen_instr_encoding_def(
        context, encoding_def_buffer, instructions[i].name->ptr, forms, n_forms
    );
    sprintf(temp_buffer, INSTR_EXEC_DEC_FMT, instructions[i].name->ptr);
    ctx_push_string(exports, temp_buffer);
    sprintf(
        temp_buffer, INSTR_EXEC_DEF_HEAD_FMT, instructions[i].name->ptr,
        instructions[i].entry_offset
    );
    _push_string(def_buffer, temp_buffer);
    _push_string(def_buffer, INSTR_EXEC_DEF_BODY);
  }
  Array_concat(dec_buffer, encoding_dec_buffer);
  Array_concat(def_buffer, encoding_def_buffer);
  releasePrimeArray(encoding_dec_buffer);
  releasePrimeArray(encoding_def_buffer);
}

constexpr char_t JUMP_KEY_DEC_FMT[] = "static const struct jump_item\n"
                                      "JUMP_KEY_TABLE[];\n";
constexpr char_t JUMP_STATE_DEC_FMT[] = "static const struct jump_state\n"
                                        "JUMP_STATE_TABLE[];\n";
void gen_jump_table_dec(Generator *generator, const Machine *) {
  ctx_push_string(declares, JUMP_KEY_DEC_FMT);
  ctx_push_string(declares, JUMP_STATE_DEC_FMT);
}
constexpr char_t JUMP_KEY_HEADER_FMT[] = "static const struct jump_item\n"
                                         "JUMP_KEY_TABLE[] = {\n";
constexpr char_t JUMP_STATE_HEADER_FMT[] = "static const struct jump_state\n"
                                           "JUMP_STATE_TABLE[] = {\n";
constexpr char_t KEY_ITEM_FMT[] = "  { .expected_type = enum_%s_%s, .next_state_index = %lu },\n";
constexpr char_t STATE_ITEM_FMT[] = "  { .count = %u, .index = %u, .fn_encoding = %s },\n";
#define val_case_item(Type, var, PREFIX)                                                \
  case enum_##Type: {                                                                   \
    const Type *var = Array_real_addr(context->var##Array, record->offset);             \
    sprintf(temp_buffer, KEY_ITEM_FMT, PREFIX, var->name->ptr, key_items[i].next_node); \
    _push_string(key_buffer, temp_buffer);                                              \
    break;                                                                              \
  }
void gen_jump_table_def(
    Generator *, const Machine *machine, Array *key_buffer, Array *state_buffer
) {
  char_t temp_buffer[512] = {};
  char_t temp2_buffer[256] = {};

  const GContext *context = machine->context;

  _push_string(key_buffer, JUMP_KEY_HEADER_FMT);
  _push_string(state_buffer, JUMP_STATE_HEADER_FMT);

  uint32_t key_count = Array_length(context->keyArray);
  const TrieKeyItem *key_items = Array_real_addr(context->keyArray, 0);
  for (uint32_t i = 0; i < key_count; i++) {
    const Record *record = Array_vert2real(context->recordArray, (REFER(Record)) key_items[i].key);
    switch (record->typeid) {
      val_case_item(Memory, mem, "MEM")
      val_case_item(Immediate, imm, "IMM")
      val_case_item(Register, reg, "REG")
      val_case_item(RegisterGroup, grp, "GRP")
      val_case_item(Set, set, "SET")
      default: {
      }
    }
  }

  uint32_t state_count = Array_length(context->stateArray);
  const TrieNodeItem *state_items = Array_real_addr(context->stateArray, 0);
  for (uint32_t i = 0; i < state_count; i++) {
    uint32_t instr_ndx = (uint32_t) ((uint64_t) state_items[i].value >> 32);
    const Instruction *instr = GContext_getInstruction(context, instr_ndx);
    uint32_t form_ndx = (uint32_t) (uint64_t) state_items[i].value;
    if (form_ndx != 0) {
      sprintf(temp2_buffer, ENCODING_NAME_FMT, instr->name->ptr, form_ndx - 1);
      sprintf(
          temp_buffer, STATE_ITEM_FMT, state_items[i].count, state_items[i].offset, temp2_buffer
      );
    } else {
      sprintf(temp_buffer, STATE_ITEM_FMT, state_items[i].count, state_items[i].offset, "nullptr");
    }
    _push_string(state_buffer, temp_buffer);
  }

  _push_string(key_buffer, "};\n");
  _push_string(state_buffer, "};\n");
}

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define setEncodingNumber(val_str)         \
  do {                                     \
    push_string("    setEncodingNumber("); \
    push_string(val_str);                  \
    push_string(");\n");                   \
  } while (false)

#define pushEncodingNumber(val_str, count_str) \
  do {                                         \
    push_string("    pushEncodingNumber(");    \
    push_string(val_str);                      \
    push_string(", ");                         \
    push_string(count_str);                    \
    push_string(");\n");                       \
  } while (false)

#define pushEncodingNumberN(val_str, count) \
  do {                                      \
    char_t count_str[0x20] = {};            \
    sprintf(count_str, "%u", count);        \
    pushEncodingNumber(val_str, count_str); \
  } while (false)

#define numberToInstrBytes(number, count) \
  do {                                    \
    char_t buf1[0x20] = {};               \
    char_t buf2[0x20] = {};               \
    sprintf(buf1, "0x%lX", number);       \
    sprintf(buf2, "%d", count);           \
    pushEncodingNumber(buf1, buf2);       \
  } while (false)

#define getDefaultMappingBit(default_bit)                      \
  do {                                                         \
    if (items->default_eval) {                                 \
      default_bit = ((uint64_t) items->default_eval->lhs) & 1; \
      default_bit = default_bit ? -1 : 0;                      \
    }                                                          \
  } while (false)

#define getMappingItem(_items, bf)                                                 \
  Array_real_addr(                                                                 \
      (_items)->itemArray,                                                         \
      ((uint32_t) (uint64_t) AVLTree_get((_items)->itemTree, (uint64_t) (bf)) - 1) \
  )

#define MAX_IDENT_LEN 64
int32_t eval_to_val(const GContext *, const Evaluable *evaluable, char_t *buffer, const Pattern *) {
  if (enum_NUMBER == evaluable->type) {
    uint64_t number = (uint64_t) evaluable->lhs;
    return sprintf(buffer, "0x%lX", number);
  }
  Variable *variable = (Variable *) evaluable->lhs;
  const Identifier *ident = variable->lhs;

  switch (evaluable->type) {
    case enum_OP_WIDTH: {
      switch (variable->type) {
        case enum_IDENTIFIER: {
          return sprintf(buffer, "%s->width", ident->ptr);
        }
        case enum_MemItem: {
          const MemItem *item = variable->rhs;
          uint32_t width = item->width;
          return sprintf(buffer, "%u", width);
        }
      }
      break;
    }
    case enum_BIT_FIELD: {
      BitField *bf = evaluable->rhs;
      uint32_t width = bf->upper - bf->lower + 1;
      // TODO: if record refers to a set there may has different behaviors, please solve it.
      if (width == 0) { return sprintf(buffer, "0"); }
      if (bf->lower == 0) {
        return sprintf(buffer, "((%s->value)&UINT_N_MAX(%d))", ident->ptr, width);
      }
      return sprintf(buffer, "((%s->value>>%d)&UINT_N_MAX(%d))", ident->ptr, bf->lower, width);
    }
    case enum_Variable: {
      switch (variable->type) {
        case enum_IDENTIFIER: {
          return sprintf(buffer, "%s->value", ident->ptr);
        }
        case enum_MemItem: {
          const MemItem *item = variable->rhs;
          uint32_t width = item->width;
          if (width == 0) { return sprintf(buffer, "0"); }
          if (item->start == 0) {
            return sprintf(buffer, "((%s->value)&UINT_N_MAX(%d))", ident->ptr, width);
          }
          return sprintf(
              buffer, "((%s->value>>%d)&UINT_N_MAX(%d))", ident->ptr, item->start, width
          );
        }
      }
    }
  }
  return -1;
}

constexpr char_t TYPE_ENUM_FMT[] = "enum_%s_%s";
#define findParameterNdxAndType(ident)                         \
  do {                                                         \
    const uint32_t length = Array_length(pattern->args);       \
    const Parameter *params = Array_first_real(pattern->args); \
    for (uint32_t i = 0; i < length; i++) {                    \
      if (Identifier_cmp(params[i].name, ident) == 0) {        \
        type = params[i].type;                                 \
        break;                                                 \
      }                                                        \
    }                                                          \
  } while (false)
#define type_case_item(Type, var, PREFIX)                                   \
  case enum_##Type: {                                                       \
    const Type *var = Array_real_addr(context->var##Array, record->offset); \
    sprintf(buffer, TYPE_ENUM_FMT, PREFIX, var->name->ptr);                 \
    break;                                                                  \
  }
void type_to_val(const GContext *context, const Identifier *ident, char_t *buffer) {
  const Record *record = GContext_findRecord(context, ident);
  switch (record->typeid) {
    type_case_item(Memory, mem, "MEM")
    type_case_item(Immediate, imm, "IMM")
    type_case_item(Register, reg, "REG")
    type_case_item(RegisterGroup, grp, "GRP")
    type_case_item(Set, set, "SET")
  }
}

#define bin_op_case_item(op, fmt)                     \
  case op: {                                          \
    sprintf(buffer, fmt, temp_buffer1, temp_buffer2); \
    break;                                            \
  }
#define sin_op_case_item(op, fmt)       \
  case op: {                            \
    sprintf(buffer, fmt, temp_buffer2); \
    break;                              \
  }
int32_t
    expr_to_val(const GContext *context, const Expr *expr, const Pattern *pattern, char_t *buffer) {
  char_t temp_buffer1[512] = {};
  char_t temp_buffer2[512] = {};
  if (expr->type < RECURSIVE_OP_MAX) {
    if (expr->lhs) {
      expr_to_val(context, expr->lhs, pattern, buffer);
      strcpy(temp_buffer1, buffer);
    }
    if (expr->rhs) {
      expr_to_val(context, expr->rhs, pattern, buffer);
      strcpy(temp_buffer2, buffer);
    }
  }
  switch (expr->type) {
    bin_op_case_item(enum_BOOL_OR, "(%s||%s)")
    bin_op_case_item(enum_BOOL_AND, "(%s&&%s)")
    sin_op_case_item(enum_BOOL_NOT, "(!%s)")
    bin_op_case_item(CB_LT, "(%s<%s)")
    bin_op_case_item(CB_LE, "(%s<=%s)")
    bin_op_case_item(CB_GT, "(%s>%s)")
    bin_op_case_item(CB_GE, "(%s>=%s)")
    bin_op_case_item(CB_EQ, "(%s==%s)")
    bin_op_case_item(CB_NE, "(%s!=%s)")
    sin_op_case_item(AS_INV, "(~%s)")
    bin_op_case_item(AB_OR, "(%s|%s)")
    bin_op_case_item(AB_AND, "(%s&%s)")
    bin_op_case_item(AB_XOR, "(%s^%s)")
    bin_op_case_item(AB_ADD, "(%s+%s)")
    bin_op_case_item(AB_SUB, "(%s-%s)")
    bin_op_case_item(AB_MUL, "(%s*%s)")
    bin_op_case_item(AB_DIV, "(%s/%s)")
    bin_op_case_item(AB_MOD, "(%s%%%s)")
    bin_op_case_item(AB_LSH, "(%s<<%s)")
    bin_op_case_item(AB_RSH, "(%s>>%s)")
    case AS_ID: {
      eval_to_val(context, expr->rhs, temp_buffer2, pattern);
      sprintf(buffer, "%s", temp_buffer2);
      break;
    }
    case CB_IN: {
      const Identifier *type = nullptr;
      const Variable *var = (Variable *) expr->lhs;
      const Identifier *supper_type = expr->rhs;
      const Identifier *ident = var->lhs;
      findParameterNdxAndType(ident);
      type_to_val(context, supper_type, temp_buffer1);
      if (var->type == enum_IDENTIFIER) {
        sprintf(buffer, "entry_type_check(%s, %s->type)", temp_buffer1, ident->ptr);
      } else if (var->type == enum_MemItem) {
        const MemItem *item = (MemItem *) var->rhs;
        const Record *record = GContext_findRecord(context, type);
        const Memory *mem = GContext_getMemory(context, record->offset);
        const MemItem *items = Array_first_real(mem->items);
        uint32_t offset = item - items;
        sprintf(buffer, "entry_type_check(%s, %s->subtypes[%d])", temp_buffer1, ident->ptr, offset);
      }
      break;
    }
    default: {
    }
  }
  return 0;
}

// TODO: codegen_mapping_item is in a recursive call chain,
//  maybe it will cause a out of memory, please solve it.
int32_t codegen_mapping_item(
    const GContext *context, Array *buffer, MappingItems *items, const BitField *bit_field,
    const Pattern *pattern, char_t *temp_buffer
) {
  const uint32_t pre_len = Array_length(buffer);
  MappingItem *item = getMappingItem(items, bit_field);
  uint64_t default_bit = 0;
  getDefaultMappingBit(default_bit);
  if (!item) {
    int len = sprintf(
        temp_buffer, "  value = numSetBits(value, %d, %d, %ld);\n", bit_field->lower,
        bit_field->upper + 1, (int64_t) default_bit
    );
    if (len > 0) { push_string(temp_buffer); }
    return (int32_t) (Array_length(buffer) - pre_len);
  }

  uint32_t bu = item->field->upper;
  uint32_t bl = item->field->lower;

  if (bl > bit_field->lower) {
    BitField lower_bf = {.lower = bit_field->lower, .upper = bl - 1};
    codegen_mapping_item(context, buffer, items, &lower_bf, pattern, temp_buffer);
  }
  if (item->type == enum_Arith_0_Expr) {
    char_t temp_buffer1[512] = {};
    expr_to_val(context, item->target, pattern, temp_buffer1);
    sprintf(temp_buffer, "    value = numSetBits(value, %d, %d, %s);\n", bl, bu + 1, temp_buffer1);
    push_string(temp_buffer);
  } else if (item->type == enum_Switchable) {
    Switchable *switchable = item->target;
    BitField bf = {.lower = bl, .upper = bu};
    codegen_switchable(context, buffer, switchable, &bf, pattern, temp_buffer);
  }
  if (bu < bit_field->upper) {
    BitField upper_bf = {.lower = bu + 1, .upper = bit_field->upper};
    codegen_mapping_item(context, buffer, items, &upper_bf, pattern, temp_buffer);
  }
  return (int32_t) (Array_length(buffer) - pre_len);
}

#define push_expr(expr, bf)                                                                     \
  do {                                                                                          \
    expr_to_val(context, expr, pattern, temp_buffer1);                                          \
    if (((uint64_t) bf) > 64) {                                                                 \
      uint32_t bl = (bf)->lower;                                                                \
      uint32_t bu = (bf)->upper;                                                                \
      sprintf(                                                                                  \
          temp_buffer, "    value = numSetBits(value, %d, %d, %s);\n", bl, bu + 1, temp_buffer1 \
      );                                                                                        \
      push_string(temp_buffer);                                                                 \
    } else {                                                                                    \
      pushEncodingNumberN(temp_buffer1, ((uint32_t) (uint64_t) bf) / 8);                        \
    }                                                                                           \
  } while (false)

int32_t codegen_switchable(
    const GContext *context, Array *buffer, const Switchable *switchable, BitField *bf,
    const Pattern *pattern, char_t *temp_buffer
) {
  char_t temp_buffer1[512] = {};
  Options *options = switchable->options;
  const Arith_0_Expr *exprs = Array_first_real(switchable->options);
  push_string("    if (");
  expr_to_val(context, switchable->expr, pattern, temp_buffer);
  push_string(temp_buffer);
  push_string(") {\n  ");
  push_expr(&exprs[0], bf);
  if (Array_length(options) > 1) {
    push_string("    } else {\n  ");
    push_expr(&exprs[1], bf);
  }
  push_string("    }\n");
  return 0;
}

int32_t codegen_layout(
    const GContext *context, Array *buffer, const Layout *layout, uint32_t width,
    const Pattern *pattern, char_t *temp_buffer
) {
  const uint32_t pre_len = Array_length(buffer);
  switch (layout->type) {
    case enum_Arith_0_Expr: {
      expr_to_val(context, layout->target, pattern, temp_buffer);
      if (width == (uint32_t) -1) {
        const Expr *expr = layout->target;
        const Evaluable *eval = expr->rhs;
        switch (eval->type) {
          case enum_OP_WIDTH: {
            pushEncodingNumberN(temp_buffer, 1);
            push_string("    size += 1;\n");
            break;
          }
          case enum_BIT_FIELD: {
            const BitField *bf = eval->rhs;
            width = ((bf->upper - bf->lower) / 8) + 1;
            pushEncodingNumberN(temp_buffer, width);
            sprintf(temp_buffer, "    size += %u;\n", width / 8);
            push_string(temp_buffer);
            break;
          }
          case enum_NUMBER: {
            width = 0;
            uint64_t num = (uint64_t) eval->lhs;
            while (num) { num >>= 3; width ++; }
            pushEncodingNumberN(temp_buffer, width);
            sprintf(temp_buffer, "    size += %u;\n", width / 8);
            push_string(temp_buffer);
            break;
          }
          case enum_Variable: {
            char_t temp2_buffer[256] = {};
            const Evaluable width_eval = {.type = enum_OP_WIDTH, .lhs = eval->lhs, .rhs = nullptr};
            eval_to_val(context, &width_eval, temp2_buffer, pattern);
            push_string("    pushEncodingNumber(");
            push_string(temp_buffer);
            push_string(", ");
            push_string(temp2_buffer);
            push_string(" / 8);\n");
            sprintf(temp_buffer, "    size += %s / 8;\n", temp2_buffer);
            push_string(temp_buffer);
            break;
          }
        }

      } else {
        pushEncodingNumberN(temp_buffer, width / 8);
        sprintf(temp_buffer, "    size += %u;\n", width / 8);
        push_string(temp_buffer);
      }
      break;
    }
    case enum_MappingItems: {
      MappingItems *items = layout->target;
      for (uint32_t i = 0; i < width; i += 64) {
        push_string("    value = 0;\n");
        BitField bf = {.lower = i, .upper = min(i + 63, width - 1)};
        codegen_mapping_item(context, buffer, items, &bf, pattern, temp_buffer);
        sprintf(temp_buffer, "    pushInstrBytes(%d);\n", min(64, width - i) / 8);
        push_string(temp_buffer);
      }
      sprintf(temp_buffer, "    size += %u;\n", width / 8);
      push_string(temp_buffer);
      break;
    }
    case enum_Switchable: {
      codegen_switchable(
          context, buffer, layout->target, (void *) (uint64_t) width, pattern, temp_buffer
      );
      sprintf(temp_buffer, "    size += %u;\n", width / 8);
      push_string(temp_buffer);
      break;
    }
  }
  return (int32_t) (Array_length(buffer) - pre_len);
}

void codegen_form_check(const GContext *context, Array *buffer, const InstrForm *form, char_t *temp_buffer) {
  FormCheck *check = form->check;
  push_string("  /* __FORM_CHECK__ */\n  if (!");
  expr_to_val(context, check->expr, form->pattern, temp_buffer);
  push_string(temp_buffer);
  push_string(") { return 0; }\n");
}

int32_t codegen_instr_part(
    const GContext *context, Array *buffer, const InstrForm *form, const InstrPart *part,
    char_t *temp_buffer
) {
  const Identifier *name = part->name;
  const uint32_t width = part->width;
  const Layout *layout = part->layout;
  const Condition *condition = part->condition;
  if (condition) {
    sprintf(temp_buffer, "  /* %s */\n  if (", name->ptr);
    push_string(temp_buffer);
    expr_to_val(context, condition->expr, form->pattern, temp_buffer);
    push_string(temp_buffer);
    push_string(") {\n");
  } else {
    sprintf(temp_buffer, "  /* %s */ {\n", name->ptr);
    push_string(temp_buffer);
  }
  codegen_layout(context, buffer, layout, width, form->pattern, temp_buffer);
  sprintf(temp_buffer, "  }\n", width / 8);
  push_string(temp_buffer);
  return 0;
}

int32_t codegen_instr_form(const GContext *context, Array *buffer, const InstrForm *form) {
  char_t temp_buffer[512] = {};
  const uint32_t pre_len = Array_length(buffer);
  const uint32_t n_parts = Array_length(form->parts);
  const InstrPart * const parts = Array_real_addr(form->parts, 0);
  if (form->check) { codegen_form_check(context, buffer, form, temp_buffer); }
  for (uint32_t i = 0; i < n_parts; i++) {
    codegen_instr_part(context, buffer, form, &parts[i], temp_buffer);
  }
  return (int32_t) (Array_length(buffer) - pre_len);
}
