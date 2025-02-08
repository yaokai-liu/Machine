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
    case enum_Variable: {
      const Variable *var = evaluable->lhs;
      if (var->type == enum_IDENTIFIER) {
        const Parameter *param = GContext_findParameter(context, var->lhs);
        const Record *record = GContext_findRecord(context, param->type);
        if (record->typeid == enum_Immediate) {
          width = GContext_getImmediate(context, record->offset)->width;
        } else {
          // means it's a register
          return 0;
        }
      } else if (var->type == enum_MemItem) {
        const MemItem *item = var->rhs;
        width = item->width;
      }
      break;
    }
    case enum_BIT_FIELD: {
      BitField *bf = evaluable->rhs;
      width = bf->upper - bf->lower + 1;
      break;
    }
    case enum_NUMBER: {
      return 0;
    }
  }

  if (l_width != width) { return -2; }
  return 0;
}
