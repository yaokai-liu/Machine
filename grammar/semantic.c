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
  if (!bit_field) { return 0; }
  const uint32_t l_width = (bit_field->upper - bit_field->lower + 1);

  uint32_t width = 0;
  switch (evaluable->type) {
    case enum_IDENTIFIER: {
      Entry *entry = GContext_findRefer(context, evaluable->lhs);
      if (entry->type == enum_Memory) {
        width = ((Memory *) entry->target)->width;
      } else if (entry->type == enum_Memory) {
        width = ((Immediate *) entry->target)->width;
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
      Entry *entry = GContext_findRefer(context, evaluable->lhs);
      Memory *memory = entry->target;
      BitField *bf = (MEM_BASE == (uint64_t) evaluable->lhs) ? memory->base : memory->offset;
      width = bf->upper - bf->lower + 1;
      break;
    }
  }

  if (l_width != width) { return -1; }
  return 0;
}
