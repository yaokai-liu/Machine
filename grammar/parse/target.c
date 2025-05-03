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
 * Filename: target.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-03
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include "avl-tree.h"
#include "context.h"
#include "enum.h"
#include "generated/tokens.gen.h"

void releaseMachine(Machine *machine, const Allocator *) {
  if (machine->context) { GContext_destroy(machine->context); }
  // TODO: release other arrays.
}

void releasePattern(Pattern *pattern, const Allocator *) {
  if (!pattern->args) { return; }
  releasePrimeArray(pattern->args);
}

void releaseSwitchable(Switchable *switchable, const Allocator *allocator) {
  releaseExpr(switchable->expr, allocator);
  allocator->free(switchable->expr);
  Array_reset(switchable->options, (destruct_t *) releaseExpr);
  Array_destroy(switchable->options);
}

void releaseMappingItem(MappingItem *item, const Allocator *allocator) {
  if (item->type == enum_Arith_0_Expr) {
    releaseExpr(item->target, allocator);
    allocator->free(item->target);
  } else if (item->type == enum_Switchable) {
    releaseSwitchable(item->target, allocator);
    allocator->free(item->target);
  }
}

void releaseMappingItems(MappingItems *items, const Allocator *allocator) {
  if (items->itemTree) { AVLTree_destroy(items->itemTree, nullptr); }
  if (items->default_eval) { allocator->free(items->default_eval); }
  Array_reset(items->itemArray, (destruct_t *) releaseMappingItem);
  Array_destroy(items->itemArray);
}

void releaseLayout(Layout *layout, const Allocator *allocator) {
  switch (layout->type) {
    case enum_Arith_0_Expr: {
      releaseExpr(layout->target, allocator);
      break;
    }
    case enum_MappingItems: {
      releaseMappingItems(layout->target, allocator);
      break;
    }
    case enum_Switchable: {
      releaseSwitchable(layout->target, allocator);
      break;
    }
  }
  allocator->free(layout->target);
}

void releaseInstrPart(InstrPart *part, const Allocator *allocator) {
  releaseLayout(part->layout, allocator);
  allocator->free(part->layout);
  if (part->condition) {
    releaseCondition(part->condition, allocator);
    allocator->free(part->condition);
  }
}

void releaseInstrForm(InstrForm *form, const Allocator *allocator) {
  releasePattern(form->pattern, allocator);
  allocator->free(form->pattern);
  if (form->check) {
    releaseCondition(form->check, allocator);
    allocator->free(form->check);
  }
  Array_reset(form->parts, (destruct_t *) releaseInstrPart);
  Array_destroy(form->parts);
}

void releaseInstruction(Instruction *instr, const Allocator *) {
  Array_reset(instr->forms, (destruct_t *) releaseInstrForm);
  Array_destroy(instr->forms);
}

void releaseMemory(Memory *memory, const Allocator *) {
  releasePrimeArray(memory->items);
}

void releaseRegisterGroup(RegisterGroup *rg, const Allocator *) {
  releasePrimeArray(rg->registers);
}

void releaseRecordSet(RecordSet *set, const Allocator *) {
  Set_destroy(set->items);
}

void releaseCondition(Condition *condition, const Allocator *allocator) {
  releaseExpr(condition->expr, allocator);
  allocator->free(condition->expr);
}

void releaseExpr(CondExpr *expr, const Allocator *allocator) {
  switch (expr->type) {
    case AS_ID:
    case AS_INV:
    case enum_BOOL_NOT: {
      releaseExpr(expr->rhs, allocator);
      allocator->free(expr->rhs);
      break;
    }
    case CB_IN:
    case enum_OP_WIDTH:
    case enum_Variable:
    case enum_BIT_FIELD: {
      allocator->free(expr->lhs);
      break;
    }
    case enum_NUMBER: {
      break;
    }
    default: {
      releaseExpr(expr->lhs, allocator);
      releaseExpr(expr->rhs, allocator);
      allocator->free(expr->lhs);
      allocator->free(expr->rhs);
      break;
    }
  }
}

inline int32_t PatternArgs_cmp(PatternArgs *args1, PatternArgs *args2) {
  if (args1 == args2) { return 0; }
  if (!args1) { return 1; }
  if (!args2) { return -1; }
  const uint32_t len1 = Array_length(args1);
  const uint32_t len2 = Array_length(args2);
  if (len1 < len2) { return -1; }
  if (len1 > len2) { return 1; }

  const Parameter * const params1 = Array_real_addr(args1, 0);
  const Parameter * const params2 = Array_real_addr(args2, 0);
  int32_t diff = 0;
  for (uint32_t i = 0; i < len1; i++) {
    const Parameter param1 = params1[i];
    const Parameter param2 = params2[i];
    diff = (int32_t) ((int64_t) param1.type - (int64_t) param2.type);
    if (diff) { return diff; }
  }
  return 0;
}
