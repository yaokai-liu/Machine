/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: semantic.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-08
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "semantic.h"
#include "enum.h"
#include "tokens.gen.h"

int32_t check_mapping_item(GContext *context, BitField *bit_field, Evaluable *evaluable) {
  if (!bit_field) { return (evaluable->type == enum_NUMBER) ? 0 : -1; }
  const uint32_t l_width = (bit_field->upper - bit_field->lower + 1);

  uint32_t width = 0;
  switch (evaluable->type) {
    case enum_IDENTIFIER: {
      Record *record = GContext_findRecord(context, evaluable->lhs);
      if (record->typeid == enum_Memory) {
        width = GContext_getMemory(context, record->offset)->width;
      } else if (record->typeid == enum_Immediate) {
        width = GContext_getImmediate(context, record->offset)->width;
      } else {
        // means it's a register
        return 0;
      }
      break;
    }
    case enum_BIT_FIELD: {
      BitField *bf = evaluable->rhs;
      width = bf->upper - bf->lower + 1;
      break;
    }
    case enum_MEM_KEY: {
      Record *record = GContext_findRecord(context, evaluable->lhs);
      Memory *memory = GContext_getMemory(context, record->offset);
      BitField *bf = (MEM_BASE == (uint64_t) evaluable->lhs) ? memory->base : memory->offset;
      width = bf->upper - bf->lower + 1;
      break;
    }
  }

  if (l_width != width) { return -2; }
  return 0;
}
