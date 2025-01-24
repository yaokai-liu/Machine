/**
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
#include "context.h"
#include "enum.h"
#include "stdint.h"
#include "terminal.h"
#include "tokens.gen.h"
#include "trie-dump.h"
#include <stdio.h>
#include <string.h>

constexpr char_t ENCODING_DEF_FMT_HEAD[] = "{\n"
                                           "  const uint32_t size = %u;\n"
                                           "  uint8_t bytes[%u] = {};\n"
                                           "  uint64_t number = 0;\n"
                                           "  uint32_t index = 0;\n";

constexpr char_t ENCODING_DEF_FMT_TAIL[] = "  Array_append(buffer, bytes, sizeof(bytes));\n"
                                           "  return size;\n"
                                           "}\n";

constexpr char_t ENCODING_FUNC_NAME_FMT[] = "encoding_%s_%u";
constexpr char_t ENUM_OPCODE_FMT[] = "enum_OP_%s";

#define _push_string(buffer, s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

#define online_gen_encoding_dec_core(form)                               \
  do {                                                                   \
    char_t func_name[sizeof(ENCODING_DEF_FMT_HEAD) + 64] = {};           \
    sprintf(func_name, "uint32_t encoding_%s_%d(", instr_op, i);         \
    push_string(func_name);                                              \
    if ((form).pattern->args) {                                          \
      const uint32_t n_args = Array_length((form).pattern->args);        \
      const Identifier *args = Array_real_addr((form).pattern->args, 0); \
      for (uint32_t j = 0; j < n_args; j++) {                            \
        const Identifier *arg = &args[j];                                \
        push_string("uint64_t ");                                        \
        push_string(arg->ptr);                                           \
        push_string(", ");                                               \
      }                                                                  \
    }                                                                    \
    push_string("Array *buffer)");                                       \
  } while (false)

int32_t online_gen_instr_encoding_dec(
    GContext *, Array *buffer, const char_t *instr_op, const InstrForm forms[], uint32_t n_forms
) {
  for (uint32_t i = 0; i < n_forms; ++i) {
    online_gen_encoding_dec_core(forms[i]);
    push_string(";\n");
  }
  return 0;
}

int32_t online_gen_instr_encoding_def(
    GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
) {
  char_t head_buffer[sizeof(ENCODING_DEF_FMT_HEAD) + 64];
  Array *temp_buffer = Array_new(sizeof(char_t), -1, context->allocator);
  for (uint32_t i = 0; i < n_forms; ++i) {
    const uint32_t n_bytes = forms[i].width / 8;
    online_gen_encoding_dec_core(forms[i]);
    sprintf(head_buffer, ENCODING_DEF_FMT_HEAD, n_bytes, n_bytes);
    push_string(head_buffer);
    codegen_instr_form(context, temp_buffer, &forms[i]);
    uint32_t size = Array_length(temp_buffer);
    char_t *ptr = Array_real_addr(temp_buffer, 0);
    Array_append(buffer, ptr, size);
    push_string(ENCODING_DEF_FMT_TAIL);
    Array_reset(temp_buffer, nullptr);
  }
  Array_destroy(temp_buffer);
  return 0;
}

int32_t online_gen_instr_encoding_op(
    GContext * /*unused*/, Array *buffer, char_t *name, const InstrForm * /*unused*/, const uint32_t
    /*unused*/
) {
  char_t temp_buffer[256] = {};
  sprintf(temp_buffer, ENUM_OPCODE_FMT, name);
  push_string(temp_buffer);
  return 0;
}

const char_t INSTR_EXEC_DEC_FMT[] = "int32_t %s(Array *buffer, ...);\n";
const char_t INSTR_EXEC_DEF_FMT[] = "int32_t %s(Array *buffer, ...) {\n"
                                    "constexpr uint32_t entry_offset = %u;\n"
                                    "}\n";
void gen_instr_exec_and_encoding(GContext *context) {
  char_t temp_buffer[512] = {};
  Array *emit_dec_buffer = GContext_getOutputBuffer(context, CtxBuf_declare);
  Array *emit_def_buffer = GContext_getOutputBuffer(context, CtxBuf_definition);
  Array *encoding_dec_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_dec);
  Array *encoding_def_buffer = GContext_getOutputBuffer(context, CtxBuf_encoding_def);

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
    _push_string(emit_dec_buffer, temp_buffer);
    sprintf(
        temp_buffer, INSTR_EXEC_DEF_FMT, instructions[i].name->ptr, instructions[i].entry_offset
    );
    _push_string(emit_def_buffer, temp_buffer);
  }
}

constexpr char_t JUMP_KEY_HEADER_FMT[] = "const static struct jump_item\n"
                                         "JUMP_TABLE_KEY_%s[] = {\n";
constexpr char_t JUMP_STATE_HEADER_FMT[] = "const static struct jump_state\n"
                                           "JUMP_TABLE_STATE_%s[] = {\n";
constexpr char_t KEY_ITEM_FMT[] = "  { .expected_type = enum_%s_%s, .next_state_index = %lu },\n";
constexpr char_t STATE_ITEM_FMT[] = "  { .count = %u, .index = %u, .fn_encoding = %s },\n";
#define val_case_item(Type, var, PREFIX)                                                \
  case enum_##Type: {                                                                   \
    const Type *var = Array_real_addr(context->var##Array, record->offset);             \
    sprintf(temp_buffer, KEY_ITEM_FMT, PREFIX, var->name->ptr, key_items[i].next_node); \
    _push_string(key_buffer, temp_buffer);                                              \
    break;                                                                              \
  }
int32_t gen_instr_encoding_mat(
    GContext *context, const Machine *machine, Array *key_buffer, Array *state_buffer
) {
  char_t temp_buffer[512] = {};
  char_t temp2_buffer[256] = {};

  sprintf(temp_buffer, JUMP_KEY_HEADER_FMT, machine->name->ptr);
  _push_string(key_buffer, temp_buffer);
  uint32_t key_count = Array_length(context->keyArray);
  const TrieKeyItem *key_items = Array_real_addr(context->keyArray, 0);
  for (uint32_t i = 0; i < key_count; i++) {
    const Record *record = Array_vert2real(context->recordArray, (REFER(Record)) key_items[i].key);
    switch (record->typeid) {
      val_case_item(Memory, mem, "MEM");
      val_case_item(Immediate, imm, "IMM");
      val_case_item(Register, reg, "REG");
      val_case_item(RegisterGroup, grp, "GRP");
      val_case_item(Set, set, "SET");
      default: {
      }
    }
  }
  _push_string(key_buffer, "};\n");

  sprintf(temp_buffer, JUMP_STATE_HEADER_FMT, machine->name->ptr);
  _push_string(state_buffer, temp_buffer);
  uint32_t state_count = Array_length(context->stateArray);
  const TrieNodeItem *state_items = Array_real_addr(context->stateArray, 0);
  for (uint32_t i = 0; i < state_count; i++) {
    uint32_t instr_ndx = (uint32_t) ((uint64_t) state_items[i].value >> 32);
    const Instruction *instr = GContext_getInstruction(context, instr_ndx);
    uint32_t form_ndx = (uint32_t) (uint64_t) state_items[i].value;
    if (form_ndx != 0) {
      sprintf(temp2_buffer, ENCODING_FUNC_NAME_FMT, instr->name->ptr, form_ndx - 1);
      sprintf(
          temp_buffer, STATE_ITEM_FMT, state_items[i].count, state_items[i].offset, temp2_buffer
      );
    } else {
      sprintf(temp_buffer, STATE_ITEM_FMT, state_items[i].count, state_items[i].offset, "nullptr");
    }
    _push_string(state_buffer, temp_buffer);
  }
  _push_string(state_buffer, "};\n");

  return 0;
}

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define setEncodingNumber(val_str)       \
  do {                                   \
    push_string("  setEncodingNumber("); \
    push_string(val_str);                \
    push_string(");\n");                 \
  } while (false)

#define pushEncodingNumber(val_str, count_str) \
  do {                                         \
    push_string("  pushEncodingNumber(");      \
    push_string(val_str);                      \
    push_string(", ");                         \
    push_string(count_str);                    \
    push_string(");\n");                       \
  } while (false)

#define pushEncodingNumberN(val_str, count) \
  do {                                      \
    char_t count_str[0x20] = {};            \
    sprintf(count_str, "%d", count);        \
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

static thread_local char_t FMT_BUFFER[1024] = {};

#define MAX_IDENT_LEN 64
int32_t eval_to_val(GContext *context, Evaluable *evaluable, char_t *buffer) {
  if (enum_NUMBER == evaluable->type) {
    uint64_t number = (uint64_t) evaluable->lhs;
    return sprintf(buffer, "0x%lX", number);
  }
  Identifier *ident = (Identifier *) evaluable->lhs;
  if (ident->len > MAX_IDENT_LEN) { return -1; }
  Record *record = GContext_findRecord(context, ident);
  switch (evaluable->type) {
    case enum_NUMBER:
    case enum_IDENTIFIER: {
      return sprintf(buffer, "%s", ident->ptr);
    }
    case enum_BIT_FIELD: {
      BitField *bf = evaluable->rhs;
      uint32_t width = bf->upper - bf->lower + 1;
      // TODO: if record refers to a set there may has different behaviors, please solve it.
      return sprintf(buffer, "(%s >> %d) & UINT_N_MAX(%d)", ident->ptr, bf->lower, width);
    }
    case enum_MEM_KEY: {
      const Memory *mem = GContext_getMemory(context, record->offset);
      BitField *bf = (((uint64_t) evaluable->rhs) == MEM_BASE) ? mem->base : mem->offset;
      uint32_t width = bf->upper - bf->lower + 1;
      return sprintf(buffer, "(%s >> %d) & UINT_N_MAX(%d)", ident->ptr, bf->lower, width);
    }
  }
  return -1;
}

int32_t codegen_items_bf(
    GContext *context, Array *buffer, MappingItems *items, const BitField *bit_field
) {
  const uint32_t pre_len = Array_length(buffer);
  MappingItem *item = getMappingItem(items, bit_field);
  uint64_t default_bit = 0;
  getDefaultMappingBit(default_bit);
  if (!item) {
    int len = sprintf(
        FMT_BUFFER, "  number = numSetBits(number, %d, %d, %ld);\n", bit_field->lower,
        bit_field->upper + 1, (int64_t) default_bit
    );
    if (len > 0) { push_string(FMT_BUFFER); }
    return Array_length(buffer) - pre_len;
  }

  uint32_t bu = item->field->upper;
  uint32_t bl = item->field->lower;

  if (bl > bit_field->lower) {
    BitField lower_bf = {.lower = bit_field->lower, .upper = bl - 1};
    codegen_items_bf(context, buffer, items, &lower_bf);
  }
  char_t *temp_buffer = nullptr;
  if (item->evaluable->type != enum_NUMBER) {
    size_t size = ((Identifier *) item->evaluable->lhs)->len;
    temp_buffer = GContext_getAllocator(context)->malloc((2 * size + 128) * sizeof(char_t));
  } else {
    temp_buffer = GContext_getAllocator(context)->malloc(128 * sizeof(char_t));
  }
  eval_to_val(context, item->evaluable, temp_buffer);
  sprintf(FMT_BUFFER, "  number = numSetBits(number, %d, %d, %s);\n", bl, bu + 1, temp_buffer);
  push_string(FMT_BUFFER);
  GContext_getAllocator(context)->free(temp_buffer);

  if (bu < bit_field->upper) {
    BitField upper_bf = {.lower = bu + 1, .upper = bit_field->upper};
    codegen_items_bf(context, buffer, items, &upper_bf);
  }
  return Array_length(buffer) - pre_len;
}

int32_t codegen_layout(GContext *context, Array *buffer, const Layout *layout, uint32_t width) {
  const uint32_t pre_len = Array_length(buffer);
  switch (layout->type) {
    case enum_Evaluable: {
      Evaluable *evaluable = layout->target;
      int32_t size = eval_to_val(context, evaluable, FMT_BUFFER);
      if (size < 0) { return size; }
      pushEncodingNumberN(FMT_BUFFER, width / 8);
      break;
    }
    case enum_MappingItems: {
      MappingItems *items = layout->target;
      for (uint32_t i = 0; i < width; i += 64) {
        BitField bf = {.lower = i, .upper = min(i + 63, width - 1)};
        codegen_items_bf(context, buffer, items, &bf);
        sprintf(FMT_BUFFER, "  pushInstrBytes(%d);\n", min(64, width - i) / 8);
        push_string(FMT_BUFFER);
        push_string("  number = 0;\n");
      }
    }
  }
  return Array_length(buffer) - pre_len;
}

#define codegen_form_part(part)                       \
  do {                                                \
    uint32_t width;                                   \
    const Layout *layout;                             \
    width = form->parts[(part) - 1].width;            \
    if (width > 0) {                                  \
      layout = form->parts[(part) - 1].layout;        \
      codegen_layout(context, buffer, layout, width); \
    }                                                 \
  } while (false)
int32_t codegen_instr_form(GContext *context, Array *buffer, const InstrForm *form) {
  const uint32_t pre_len = Array_length(buffer);
  codegen_form_part(PART_PREFIX);
  codegen_form_part(PART_PRINCIPAL);
  codegen_form_part(PART_SUFFIX);
  return Array_length(buffer) - pre_len;
}
