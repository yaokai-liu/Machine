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
 * Module Name: grammar/parse
 * Filename: semantic.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-08
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "semantic.h"
#include "enum.h"
#include "generated/tokens.gen.h"

int32_t check_mapping_item_evaluable(
    ParseContext *context, BitField *bit_field, const Evaluable *evaluable
);

int32_t check_mapping_item(ParseContext *context, BitField *bit_field, const Arith_0_Expr *expr) {
  if (expr->type == AS_ID) { return check_mapping_item_evaluable(context, bit_field, expr->rhs); }
  int32_t result = 0;
  if (expr->lhs) { result = check_mapping_item(context, bit_field, expr->lhs); }
  result += check_mapping_item(context, bit_field, expr->rhs);
  return result;
}

int32_t check_mapping_item_evaluable(
    ParseContext *context, BitField *bit_field, const Evaluable *evaluable
) {
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

  if (l_width > width) { return -2; }
  return 0;
}
