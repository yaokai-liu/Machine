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
#include "terminal.h"

void releaseEntry(Entry *, const Allocator *) {}

void releaseMachine(Machine *machine, const Allocator *allocator) {
  releaseIdentifier(machine->name, allocator);
  allocator->free(machine->name);
  if (machine->context) { GContext_destroy(machine->context); }
  // TODO: release other arrays.
}

void releaseImmediate(Immediate *immediate, const Allocator *allocator) {
  releaseIdentifier(immediate->name, allocator);
  allocator->free(immediate->name);
}

void releaseParameter(Parameter *parameter, const Allocator *allocator) {
  releaseIdentifier(parameter->type, allocator);
  releaseIdentifier(parameter->name, allocator);
  allocator->free(parameter->type);
  allocator->free(parameter->name);
}

void releasePattern(Pattern *pattern, const Allocator *) {
  if (!pattern->args) { return; }
  Array_reset(pattern->args, (destruct_t *) releaseParameter);
  Array_destroy(pattern->args);
}
void releaseVariable(Variable *variable, const Allocator *allocator) {
  releaseIdentifier(variable->lhs, allocator);
  allocator->free(variable->lhs);
}

void releaseEvaluable(Evaluable *evaluable, const Allocator *allocator) {
  if (enum_NUMBER == evaluable->type) { return; }
  if (enum_BIT_FIELD == evaluable->type) {
    releaseBitField(evaluable->rhs, allocator);
    allocator->free(evaluable->rhs);
  }
  releaseVariable(evaluable->lhs, allocator);
  allocator->free(evaluable->lhs);
}

void releaseSwitchable(Switchable *switchable, const Allocator *allocator) {
  releaseExpr(switchable->expr, allocator);
  allocator->free(switchable->expr);
  Array_reset(switchable->options, (destruct_t *) releaseExpr);
  Array_destroy(switchable->options);
}

void releaseMappingItem(MappingItem *item, const Allocator *allocator) {
  releaseBitField(item->field, allocator);
  allocator->free(item->field);
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
  if (items->default_eval) {
    releaseEvaluable(items->default_eval, allocator);
    allocator->free(items->default_eval);
  }
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
  releaseIdentifier(part->name, allocator);
  allocator->free(part->name);
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

void releaseInstruction(Instruction *instr, const Allocator *allocator) {
  releaseIdentifier(instr->name, allocator);
  allocator->free(instr->name);
  Array_reset(instr->forms, (destruct_t *) releaseInstrForm);
  Array_destroy(instr->forms);
}

void releaseMemItem(MemItem *item, const Allocator *allocator) {
  releaseIdentifier(item->name, allocator);
  allocator->free(item->name);
  if (item->type) {
    releaseIdentifier(item->type, allocator);
    allocator->free(item->type);
  }
}

void releaseMemory(Memory *memory, const Allocator *allocator) {
  releaseIdentifier(memory->name, allocator);
  allocator->free(memory->name);
  Array_reset(memory->items, (destruct_t *) releaseMemItem);
  Array_destroy(memory->items);
}

void releaseRegister(Register *reg, const Allocator *allocator) {
  releaseIdentifier(reg->name, allocator);
  releaseBitField(reg->field, allocator);
  allocator->free(reg->name);
  allocator->free(reg->field);
}

void releaseRegisterGroup(RegisterGroup *rg, const Allocator *allocator) {
  releaseIdentifier(rg->name, allocator);
  allocator->free(rg->name);
  releasePrimeArray(rg->registers);
}

void releaseSet(Set *set, const Allocator *allocator) {
  releaseIdentifier(set->name, allocator);
  allocator->free(set->name);
  Array_reset(set->items, (destruct_t *) releaseIdentifier);
  Array_destroy(set->items);
}

void releaseCondition(Condition *condition, const Allocator *allocator) {
  releaseExpr(condition->expr, allocator);
  allocator->free(condition->expr);
}

void releaseExpr(CondExpr *expr, const Allocator *allocator) {
  switch (expr->type) {
    case AS_ID: {
      releaseEvaluable(expr->rhs, allocator);
      allocator->free(expr->rhs);
      break;
    }
    case AS_INV:
    case enum_BOOL_NOT: {
      releaseExpr(expr->rhs, allocator);
      allocator->free(expr->rhs);
      break;
    }
    case CB_IN: {
      releaseVariable(expr->lhs, allocator);
      releaseIdentifier(expr->rhs, allocator);
      allocator->free(expr->lhs);
      allocator->free(expr->rhs);
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

#include "string_t.h"

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
  for (uint32_t i = 0; i < len1; i++) {
    const Parameter * const param1 = &params1[i];
    const Parameter * const param2 = &params2[i];
    int32_t b =
        !Identifier_cmp(param1->type, param2->type) && !Identifier_cmp(param1->name, param2->name);
    if (!b) { return 1; }
  }
  return 0;
}
