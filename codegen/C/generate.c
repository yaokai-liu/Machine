/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generate.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "generate.h"
#include "encoding.h"
#include "array.h"
#include "codegen.h"
#include "context.h"
#include "enum.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

#define setEncodingNumber(val_str)     \
  do {                                 \
    push_string("setEncodingNumber("); \
    push_string(val_str);              \
    push_string(");");                 \
  } while (false)

#define pushEncodingNumber(val_str, count_str) \
  do {                                         \
    push_string("pushEncodingNumber(");        \
    push_string(val_str);                      \
    push_string(", ");                         \
    push_string(count_str);                    \
    push_string(");");                         \
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
  Array_get(                                                                       \
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
  Entry *entry = GContext_findRefer(context, ident);
  switch (evaluable->type) {
    case enum_NUMBER:
    case enum_IDENTIFIER: {
      if (entry->type == enum_Set) {
        // TODO: codegen for set
        return -1;
      } else {
        return sprintf(buffer, "%s", ident->ptr);
      }
    }
    case enum_BIT_FIELD: {
      BitField *bf = evaluable->rhs;
      uint32_t width = bf->upper - bf->lower + 1;
      if (entry->type == enum_Set) {
        // TODO: codegen for set
        return -1;
      } else {
        return sprintf(buffer, "(%s >> %d) & UINT_N_MAX(%d)", ident->ptr, bf->lower, width);
      }
    }
    case enum_MEM_KEY: {
      Memory *mem = entry->target;
      BitField *bf = (((uint64_t) evaluable->rhs) == MEM_BASE) ? mem->base : mem->offset;
      uint32_t width = bf->upper - bf->lower + 1;
      return sprintf(buffer, "(%s >> %d) & UINT_N_MAX(%d)", ident->ptr, bf->lower, width);
    }
  }
  return -1;
}
int32_t codegen_items_bf(GContext *context, MappingItems *items, const BitField *bf, Array *buffer);

int32_t
    codegen_items_bf(GContext *context, MappingItems *items, const BitField *bf, Array *buffer) {
  const uint32_t pre_len = Array_length(buffer);
  MappingItem *item = getMappingItem(items, bf);
  uint64_t default_bit = 0;
  getDefaultMappingBit(default_bit);
  if (!item) {
    int len = sprintf(
        FMT_BUFFER, "number = numSetBits(number, %d, %d, %ld);", bf->lower, bf->upper + 1,
        (int64_t) default_bit
    );
    if (len > 0) { push_string(FMT_BUFFER); }
    return Array_length(buffer) - pre_len;
  }

  uint32_t bu = item->field->upper;
  uint32_t bl = item->field->lower;

  if (bl > bf->lower) {
    BitField lower_bf = {.lower = bf->lower, .upper = bl - 1};
    codegen_items_bf(context, items, &lower_bf, buffer);
  }
  char_t *temp_buffer = nullptr;
  if (item->evaluable->type != enum_NUMBER) {
    size_t size = ((Identifier *) item->evaluable->lhs)->len;
    temp_buffer = GContext_getAllocator(context)->malloc((2 * size + 128) * sizeof(char_t));
  } else {
    temp_buffer = GContext_getAllocator(context)->malloc(128 * sizeof(char_t));
  }
  eval_to_val(context, item->evaluable, temp_buffer);
  sprintf(FMT_BUFFER, "number = numSetBits(number, %d, %d, %s);", bl, bu + 1, temp_buffer);
  push_string(FMT_BUFFER);
  GContext_getAllocator(context)->free(temp_buffer);

  if (bu < bf->upper) {
    BitField upper_bf = {.lower = bu + 1, .upper = bf->upper};
    codegen_items_bf(context, items, &upper_bf, buffer);
  }
  return Array_length(buffer) - pre_len;
}

int32_t codegen_layout(GContext *context, const Layout *layout, uint32_t width, Array *buffer) {
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
        codegen_items_bf(context, items, &bf, buffer);
        sprintf(FMT_BUFFER, "pushInstrBytes(%d);", min(64, width - i) / 8);
        push_string(FMT_BUFFER);
        push_string("number = 0;");
      }
    }
  }
  return Array_length(buffer) - pre_len;
}

int32_t codegen_instr_form(GContext *context, InstrForm *form, Array *buffer) {
  const uint32_t pre_len = Array_length(buffer);
  uint32_t width;
  const Layout *layout;
  width = form->parts[PART_PREFIX - 1].width;
  layout = form->parts[PART_PREFIX - 1].layout;
  codegen_layout(context, layout, width, buffer);
  width = form->parts[PART_PRINCIPAL - 1].width;
  layout = form->parts[PART_PRINCIPAL - 1].layout;
  codegen_layout(context, layout, width, buffer);
  width = form->parts[PART_SUFFIX - 1].width;
  layout = form->parts[PART_SUFFIX - 1].layout;
  codegen_layout(context, layout, width, buffer);
  char_t c = '\0';
  Array_append(buffer, &c, 1);
  return Array_length(buffer) - pre_len;
}

//int32_t codegen_instruction(GContext * context, Instruction * instr, Array *buffer) {
//
//  return 0;
//}

codegen_t *get_codegen(uint32_t type) {
  switch (type) {
    case enum_InstrForm: {
      return (codegen_t *) codegen_instr_form;
    }
  }
  return nullptr;
}
