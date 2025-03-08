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
 * Filename: reduce.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-15
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "action-table.gen.h"
#include "array.h"
#include "avl-tree.h"
#include "context.h"
#include "enum.h"
#include "expr-reduce.h"
#include "semantic.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"
#include <stdint.h>

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define grammarAssertDefinedRecord(ident)                         \
  do {                                                            \
    const Record *record = GContext_findRecord(context, ident);   \
    if (!record) {                                                \
      GContext_setErrorMessage(context, "undefined identifier."); \
      return nullptr;                                             \
    }                                                             \
  } while (false)

#define grammarAssertHasArgument(ident)                       \
  do {                                                        \
    if (!GContext_findParameter(context, ident)) {            \
      GContext_setErrorMessage(context, "no such variable."); \
      return nullptr;                                         \
    }                                                         \
  } while (false)

#define grammarAssertNotDeclaredRecord(ident)                     \
  do {                                                            \
    const Record *record = GContext_findRecord(context, ident);   \
    if (record) {                                                 \
      GContext_setErrorMessage(context, "redefined identifier."); \
      return nullptr;                                             \
    }                                                             \
    void *id = GContext_findIdentInStack(context, ident);         \
    if (id) {                                                     \
      GContext_setErrorMessage(context, "redefined identifier."); \
      return nullptr;                                             \
    }                                                             \
  } while (false)

#define grammarAssertNotDeclaredInstrPart(ident)                 \
  do {                                                           \
    if (GContext_findInstrPart(context, ident)) {                \
      GContext_setErrorMessage(context, "redefined InstrPart."); \
      return nullptr;                                            \
    }                                                            \
  } while (false)

#define grammarAssertNotDeclaredMemItem(ident)                 \
  do {                                                         \
    if (GContext_findMemItem(context, ident)) {                \
      GContext_setErrorMessage(context, "redefined MemItem."); \
      return nullptr;                                          \
    }                                                          \
  } while (false)

#define grammarAssertNotDeclaredOpcode(ident)                     \
  do {                                                            \
    Instruction *instr = GContext_findOpcode(context, ident);     \
    if (instr) {                                                  \
      GContext_setErrorMessage(context, "redefined identifier."); \
      return nullptr;                                             \
    }                                                             \
  } while (false)

#define grammarAssert(bool_expr, msg)         \
  do {                                        \
    if (!(bool_expr)) {                       \
      GContext_setErrorMessage(context, msg); \
      return nullptr;                         \
    }                                         \
  } while (false)

Arith_0_Expr *p_Arith_0_Expr_0(void *argv[], GContext *, const Allocator *allocator) {
  Arith_0_Expr *lhs = (Arith_0_Expr *) argv[0];
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1];
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2];

  Arith_0_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
Arith_0_Expr *p_Arith_0_Expr_1(void *argv[], GContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0];
  Arith_0_Expr *rhs = (Arith_1_Expr *) argv[1];

  Arith_0_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_0_Expr *p_Arith_0_Expr_2(void *argv[], GContext *, const Allocator *) {
  return (Arith_1_Expr *) argv[0];
}

Arith_1_Expr *p_Arith_1_Expr_0(void *argv[], GContext *, const Allocator *allocator) {
  Arith_1_Expr *lhs = (Arith_1_Expr *) argv[0];
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1];
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2];

  Arith_1_Expr *expr = allocator->calloc(1, sizeof(Arith_1_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
Arith_1_Expr *p_Arith_1_Expr_1(void *argv[], GContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0];
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[1];

  Arith_1_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_1_Expr *p_Arith_1_Expr_2(void *argv[], GContext *, const Allocator *) {
  return (Arith_1_Expr *) argv[0];
}

Arith_2_Expr *p_Arith_2_Expr_0(void *argv[], GContext *, const Allocator *allocator) {
  Arith_2_Expr *lhs = (Arith_2_Expr *) argv[0];
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1];
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2];

  Arith_2_Expr *expr = allocator->calloc(1, sizeof(Arith_2_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
Arith_2_Expr *p_Arith_2_Expr_1(void *argv[], GContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0];
  Arith_2_Expr *rhs = (Arith_1_Expr *) argv[1];

  Arith_2_Expr *expr = allocator->calloc(1, sizeof(Arith_2_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_2_Expr *p_Arith_2_Expr_2(void *argv[], GContext *, const Allocator *) {
  return (Arith_2_Expr *) argv[0];
}
Arith_3_Expr *p_Arith_3_Expr_0(void *argv[], GContext *, const Allocator *) {
  Arith_0_Expr *expr = (Arith_0_Expr *) argv[1];
  return (Arith_3_Expr *) expr;
}
Arith_3_Expr *p_Arith_3_Expr_1(void *argv[], GContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0];
  Arith_3_Expr *rhs = (Arith_1_Expr *) argv[1];

  Arith_3_Expr *expr = allocator->calloc(1, sizeof(Arith_3_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_3_Expr *p_Arith_3_Expr_2(void *argv[], GContext *, const Allocator *allocator) {
  Evaluable *eval = (Evaluable *) argv[0];
  Arith_3_Expr *expr = allocator->calloc(1, sizeof(Arith_3_Expr));
  expr->type = AS_ID;
  expr->lhs = nullptr;
  expr->rhs = eval;
  return expr;
}

Condition *p_Condition_0(void *argv[], GContext *, const Allocator *allocator) {
  CondExpr *expr = (CondExpr *) argv[2];
  Condition *cond = allocator->calloc(1, sizeof(Condition));
  cond->expr = expr;
  return cond;
}
CondExpr *p_CondExpr_0(void *argv[], GContext *, const Allocator *allocator) {
  CondExpr *lhs = (CondExpr *) argv[0];
  AndCondExpr *rhs = (AndCondExpr *) argv[2];

  CondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_OR;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}

CondExpr *p_CondExpr_1(void *argv[], GContext *, const Allocator *) {
  return (CondExpr *) argv[0];
}

AndCondExpr *p_AndCondExpr_0(void *argv[], GContext *, const Allocator *allocator) {
  AndCondExpr *lhs = (AndCondExpr *) argv[0];
  SingleCondExpr *rhs = (SingleCondExpr *) argv[2];

  AndCondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_AND;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
AndCondExpr *p_AndCondExpr_1(void *argv[], GContext *, const Allocator *) {
  return (CondExpr *) argv[0];
}
SingleCondExpr *p_SingleCondExpr_0(void *argv[], GContext *, const Allocator *) {
  return (CondExpr *) argv[1];
}
CondExpr *p_SingleCondExpr_1(void *argv[], GContext *, const Allocator *allocator) {
  SingleCondExpr *rhs = (SingleCondExpr *) argv[1];

  CondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_NOT;
  expr->lhs = nullptr;
  expr->rhs = rhs;

  expr = optimize_not_expr(expr, allocator);

  return expr;
}
SingleCondExpr *p_SingleCondExpr_2(void *argv[], GContext *, const Allocator *allocator) {
  SingleCondExpr *lhs = (SingleCondExpr *) argv[0];
  uint32_t type = (uint32_t) (uint64_t) argv[1];
  SingleCondExpr *rhs = (SingleCondExpr *) argv[2];

  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = type;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *p_SingleCondExpr_3(void *argv[], GContext *context, const Allocator *allocator) {
  Variable *lhs = (Variable *) argv[0];
  Identifier *rhs = (Identifier *) argv[2];

  grammarAssertDefinedRecord(rhs);
  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = CB_IN;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *p_SingleCondExpr_4(void *argv[], GContext *, const Allocator *allocator) {
  uint32_t type = (uint32_t) (uint64_t) argv[0];
  Arith_0_Expr *rhs = (Arith_0_Expr *) argv[1];

  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = type;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *p_SingleCondExpr_5(void *argv[], GContext *, const Allocator *) {
  return (Arith_0_Expr *) argv[0];
}

Options *p_Options_0(void *argv[], GContext *, const Allocator *allocator) {
  Options *options = (Options *) argv[0];
  Arith_0_Expr *evaluable = (Arith_0_Expr *) argv[2];

  Array_append(options, evaluable, 1);
  allocator->free(evaluable);
  return options;
}

Options *p_Options_1(void *argv[], GContext *, const Allocator *allocator) {
  Arith_0_Expr *evaluable = (Arith_0_Expr *) argv[0];

  Options *options = Array_new(sizeof(Arith_0_Expr), enum_Arith_0_Expr, allocator);
  Array_append(options, evaluable, 1);
  allocator->free(evaluable);
  return options;
}

Switchable *p_Switchable_0(void *argv[], GContext *, const Allocator *allocator) {
  CondExpr *expr = (CondExpr *) argv[1];
  Options *options = (Options *) argv[4];

  Switchable *switchable = allocator->calloc(1, sizeof(Switchable));
  switchable->expr = expr;
  switchable->options = options;
  return switchable;
}

Entries *p_Entries_0(void *[], GContext *, const Allocator *) {
  return (REFER(Entries))(uint64_t) (enum_Entries);
}

Entries *p_Entries_1(void *[], GContext *, const Allocator *) {
  return (REFER(Entries))(uint64_t) (enum_Entries);
}

Entry *p_Entry_0(void *[], GContext *, const Allocator *) {
  return (REFER(Entry))(uint64_t) (enum_Entry);
}

Entry *p_Entry_1(void *[], GContext *, const Allocator *) {
  return (REFER(Entry))(uint64_t) (enum_Entry);
}

Entry *p_Entry_2(void *[], GContext *, const Allocator *) {
  return (REFER(Entry))(uint64_t) (enum_Entry);
}

Entry *p_Entry_3(void *[], GContext *, const Allocator *) {
  return (REFER(Entry))(uint64_t) (enum_Entry);
}

Entry *p_Entry_4(void *[], GContext *, const Allocator *) {
  return (REFER(Entry))(uint64_t) (enum_Entry);
}

Variable *p_Variable_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0];
  Identifier *rhs = (Identifier *) argv[2];

  Parameter *param = (Parameter *) GContext_findParameter(context, lhs);
  grammarAssert(param, "no such variable.");
  param->used = true;
  const Record *record = GContext_findRecord(context, param->type);
  grammarAssert(record->typeid == enum_Memory, "Identifier is not accessible.");
  const Memory *memory = GContext_getMemory(context, record->offset);
  Gcontext_setItems(context, memory->items);
  const REFER(MemItem) item = GContext_findMemItem(context, rhs);
  grammarAssert(item, "no such field.");
  Gcontext_setItems(context, nullptr);
  releaseIdentifier(rhs, allocator);
  allocator->free(rhs);

  Variable *var = allocator->calloc(1, sizeof(Variable));
  var->type = enum_MemItem;
  var->lhs = lhs;
  var->rhs = Array_vert2real(memory->items, item);
  return var;
}

Variable *p_Variable_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];

  const Record *record = GContext_findRecord(context, ident);
  if (record) {
    Variable *var = allocator->calloc(1, sizeof(Variable));
    var->type = record->typeid;
    var->lhs = ident;
    switch (record->typeid) {
      case enum_Register: {
        const Register *reg = GContext_getRegister(context, record->offset);
        var->rhs = (void *) reg;
        break;
      }
      case enum_Memory: {
        const Memory *memory = GContext_getMemory(context, record->offset);
        var->rhs = (void *) memory;
        break;
      }
      case enum_Immediate: {
        const Immediate *immediate = GContext_getImmediate(context, record->offset);
        var->rhs = (void *) immediate;
        break;
      }
    }
    return var;
  }
  Parameter *param = (Parameter *) GContext_findParameter(context, ident);
  grammarAssert(param, "no such variable.");
  param->used = true;
  Variable *var = allocator->calloc(1, sizeof(Variable));
  var->type = enum_IDENTIFIER;
  var->lhs = ident;
  var->rhs = nullptr;
  return var;
}

Evaluable *p_Evaluable_0(void *argv[], GContext *context, const Allocator *allocator) {
  Variable *lhs = (Variable *) argv[0];
  BitField *rhs = (BitField *) argv[1];

  const Record *record = nullptr;
  Pattern *pattern = *(Pattern **) Array_last_real(context->patterns);
  Parameter *first = Array_first_real(pattern->args);
  Parameter *last = Array_last_real(pattern->args);
  for (Parameter *param = first; param <= last; param++) {
    if (Identifier_cmp(param->name, lhs->lhs) == 0) {
      record = GContext_findRecord(context, param->type);
    }
  }
  grammarAssert(record, "undeclared variable.");
  switch (lhs->type) {
    case enum_MemItem: {
      const MemItem *item = lhs->rhs;
      grammarAssert(rhs->upper < (uint64_t) item->width, "field out of range.");
      break;
    }
    case enum_IDENTIFIER: {
      if (record->typeid == enum_Immediate) {
        const Immediate *imm = GContext_getImmediate(context, record->offset);
        grammarAssert(rhs->upper < imm->width, "field out of range.");
      }
    }
  }

  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_BIT_FIELD;
  evaluable->lhs = lhs;
  evaluable->rhs = rhs;
  return evaluable;
}

Evaluable *p_Evaluable_1(void *argv[], GContext *, const Allocator *allocator) {
  Variable *var = (Variable *) argv[1];
  uint32_t width = 0;
  switch (var->type) {
    case VT_IDENTIFIER: {
      Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
      evaluable->type = enum_OP_WIDTH;
      evaluable->lhs = var;
      evaluable->rhs = nullptr;
      return evaluable;
    }
    case VT_REGISTER:{
      const Register *reg = var->rhs;
      width = reg->field->upper - reg->field->lower + 1;
      break;
    }
    case VT_MEMORY:{
      const Memory *mem = var->rhs;
      width = mem->width;
      break;
    }
    case VT_IMMEDIATE: {
      const Immediate *imm = var->rhs;
      width = imm->width;
      break;
    }
    case VT_MEM_ITEM: {
      const MemItem *item = var->rhs;
      width = item->width;
      break;
    }
    default: { releaseVariable(var, allocator); return nullptr; }
  }
  releaseVariable(var, allocator);
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) (uint64_t) width;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *p_Evaluable_2(void *argv[], GContext *context, const Allocator *allocator) {
  Variable *var = (Variable *) argv[0];

  if (var->type == VT_REGISTER) {
    const Register *reg = var->rhs;
    releaseVariable(var, allocator);
    Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
    evaluable->type = enum_NUMBER;
    evaluable->lhs = (void *) reg->code;
    evaluable->rhs = nullptr;
    return evaluable;
  }
  grammarAssert(var->type != VT_MEMORY && var->type != VT_IMMEDIATE,
                "arithmetic operation with 'Memory' or 'Immediate' entity is not supported.");
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_Variable;
  evaluable->lhs = var;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *p_Evaluable_3(void *argv[], GContext *, const Allocator *allocator) {
  uint64_t number = (uint64_t) argv[0];
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) number;
  evaluable->rhs = nullptr;
  return evaluable;
}

Immediate *p_Immediate_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  uint32_t type = (uint64_t) argv[3];

  grammarAssertNotDeclaredRecord(ident);
  grammarAssert(width <= 64, "too long to support this width.");

  Immediate imm = {.type = type, .width = width, .name = ident};

  Immediate *result = GContext_addImmediate(context, &imm);

  return result;
}

InstrForm *p_InstrForm_0(void *argv[], GContext *, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  FormCheck *check = (FormCheck *) argv[3];
  InstrParts *part_array = (InstrParts *) argv[4];
  uint32_t width = 0;
  const InstrPart *first = Array_first_real(part_array);
  const InstrPart *last = Array_last_real(part_array);
  for (const InstrPart *part = first; part <= last; part++) { width += part->width > 256 ? 256 : part->width;}

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = 1;
  form->pattern = pattern;
  form->check = (check == (void *) enum_FormCheck) ? nullptr : check;
  form->parts = part_array;

  return form;
}

InstrForm *p_InstrForm_1(void *argv[], GContext *, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0];
  uint32_t tick = (uint32_t) (uint64_t) argv[3];
  FormCheck *check = (FormCheck *) argv[4];
  InstrParts *part_array = (InstrParts *) argv[5];

  uint32_t width = 0;
  const InstrPart *first = Array_first_real(part_array);
  const InstrPart *last = Array_last_real(part_array);
  for (const InstrPart *part = first; part <= last; part++) { width += part->width > 256 ? 256 : part->width;}

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = tick;
  form->pattern = pattern;
  form->check = (check == (void *) enum_FormCheck) ? nullptr : check;
  form->parts = part_array;
  return form;
}

InstrForms *p_InstrForms_0(void *argv[], GContext *, const Allocator *allocator) {
  InstrForms *forms = (InstrForms *) argv[0];
  InstrForm *form = (InstrForm *) argv[1];
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrForms *p_InstrForms_1(void *argv[], GContext *, const Allocator *allocator) {
  InstrForm *form = (InstrForm *) argv[0];
  InstrForms *forms = Array_new(sizeof(InstrForm), enum_InstrForm, allocator);
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

FormCheck *p_FormCheck_0(void *argv[], GContext *, const Allocator *) {
  FormCheck *check = (FormCheck *) argv[0];
  return check;
}

FormCheck *p_FormCheck_1(void *[], GContext *, const Allocator *) {
  return (FormCheck *) (uint64_t) enum_FormCheck;
}

InstrPart *p_InstrPart_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Layout *layout = (Layout *) argv[4];

  grammarAssert(width != 0, "empty part.");

  if (width == (uint32_t) -1) {
    const Arith_0_Expr *expr = layout->target;
    grammarAssert(layout->type == enum_Arith_0_Expr && expr->type == AS_ID,
                  "target is too complex to calculate width.");
  } else {
    grammarAssert(width % 8 == 0,
                  "illegal width of parts. width must be an integer multiple of a byte with."
    );
  }
  grammarAssertNotDeclaredInstrPart(name);

  InstrPart *part = allocator->calloc(1, sizeof(InstrPart));
  part->name = name;
  part->width = width;
  part->layout = layout;
  part->condition = nullptr;
  return part;
}

InstrPart *p_InstrPart_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Layout *layout = (Layout *) argv[4];
  Condition *condition = (Condition *) argv[5];

  grammarAssert(width != 0, "empty part.");

  if (width == (uint32_t) -1) {
    const Arith_0_Expr *expr = layout->target;
    grammarAssert(layout->type == enum_Arith_0_Expr && expr->type == AS_ID,
                  "target is too complex to calculate width.");
  } else {
    grammarAssert(width % 8 == 0,
                  "illegal width of parts. width must be an integer multiple of a byte with."
    );
  }
  grammarAssertNotDeclaredInstrPart(name);

  InstrPart *part = allocator->calloc(1, sizeof(InstrPart));
  part->name = name;
  part->width = width;
  part->layout = layout;
  part->condition = condition;
  return part;
}

InstrParts *p_InstrParts_0(void *argv[], GContext *, const Allocator *allocator) {
  InstrParts *parts = (InstrParts *) argv[0];
  InstrPart *part = (InstrPart *) argv[1];
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

InstrParts *p_InstrParts_1(void *argv[], GContext *context, const Allocator *allocator) {
  InstrPart *part = (InstrPart *) argv[0];
  InstrParts *parts = Array_new(sizeof(InstrPart), enum_InstrPart, allocator);
  Array_append(parts, part, 1);
  allocator->free(part);

  Gcontext_setParts(context, parts);

  return parts;
}

Instruction *p_Instruction_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *identifier = (Identifier *) argv[1];
  InstrForms *forms = (InstrForms *) argv[3];

  grammarAssertNotDeclaredOpcode(identifier);

  Instruction instruction = {.name = identifier, .forms = forms};
  REFER(Instruction) instr = GContext_addInstruction(context, &instruction);
  GContext_dump_instruction(context, instr);
  return instr;
}

Layout *p_Layout_0(void *argv[], GContext *, const Allocator *allocator) {
  Arith_0_Expr *expr = (Arith_0_Expr *) argv[0];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Arith_0_Expr;
  layout->target = expr;
  return layout;
}

Layout *p_Layout_1(void *argv[], GContext *, const Allocator *allocator) {
  Switchable *switchable = (Switchable *) argv[0];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Switchable;
  layout->target = switchable;
  return layout;
}

Layout *p_Layout_2(void *argv[], GContext *, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[1];
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_MappingItems;
  layout->target = items;
  return layout;
}

Machine *p_Machine_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1];
  Machine *machine = allocator->calloc(1, sizeof(Machine));
  machine->name = identifier;
  machine->context = context;

  return machine;
}

Machine *p___EXTEND_RULE__(void *argv[], const Allocator *) {
  return (Machine *) argv[0];
}

MappingItem *p_MappingItem_0(void *argv[], GContext *context, const Allocator *allocator) {
  BitField *bit_field = (BitField *) argv[0];
  Arith_0_Expr *expr = (Arith_0_Expr *) argv[2];

  if (bit_field) {
    uint64_t width = GContext_getLastWidth(context);
    if (bit_field->upper > width) {
      GContext_setErrorMessage(context, "overflow bits.");
      return nullptr;
    }
  }
  if (GContext_getMapItem(context, bit_field)) {
    GContext_setErrorMessage(context, "rewrite bits.");
    return nullptr;
  }
  if (0 != check_mapping_item(context, bit_field, expr)) {
    GContext_setErrorMessage(context, "bit filed width mismatch.");
    return nullptr;
  }

  MappingItem *item = allocator->calloc(1, sizeof(MappingItem));
  item->type = enum_Arith_0_Expr;
  item->field = bit_field;
  item->target = expr;

  GContext_addMapItem(context, item);

  return item;
}
MappingItem *p_MappingItem_1(void *argv[], GContext *context, const Allocator *allocator) {
  BitField *bit_field = (BitField *) argv[0];
  Switchable *switchable = (Switchable *) argv[2];

  if (bit_field) {
    uint64_t width = GContext_getLastWidth(context);
    if (bit_field->upper > width) {
      GContext_setErrorMessage(context, "overflow bits.");
      return nullptr;
    }
  }
  if (GContext_getMapItem(context, bit_field)) {
    GContext_setErrorMessage(context, "rewrite bits.");
    return nullptr;
  }
  const Arith_0_Expr *first = Array_first_real(switchable->options);
  const Arith_0_Expr *last = Array_last_real(switchable->options);
  for (const Arith_0_Expr *expr = first; expr <= last; expr++) {
    if (0 != check_mapping_item(context, bit_field, expr)) {
      GContext_setErrorMessage(context, "bit filed width mismatch.");
      return nullptr;
    }
  }
  MappingItem *item = allocator->calloc(1, sizeof(MappingItem));
  item->type = enum_Switchable;
  item->field = bit_field;
  item->target = switchable;

  GContext_addMapItem(context, item);

  return item;
}

MappingItems *p_MappingItems_0(void *argv[], GContext *context, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[0];
  MappingItem *item = (MappingItem *) argv[2];

  if (!item->field) {
    if (items->default_eval) {
      GContext_setErrorMessage(context, "redefine default bits.");
      return nullptr;
    }
    items->default_eval = item->target;
  } else {
    items->lowest = min(item->field->lower, items->lowest);
    uint32_t index = Array_length(items->itemArray);
    AVLTree_set(items->itemTree, (uint64_t) item->field, (void *) (uint64_t) index + 1);
    Array_append(items->itemArray, item, 1);
  }
  allocator->free(item);
  return items;
}

MappingItems *p_MappingItems_1(void *argv[], GContext *, const Allocator *allocator) {
  MappingItem *item = (MappingItem *) argv[0];

  MappingItems *items = allocator->calloc(1, sizeof(MappingItems));
  items->itemArray = Array_new(sizeof(MappingItem), enum_MappingItem, allocator);
  items->itemTree = AVLTree_new(allocator, (compare_t *) BitField_cmp);
  if (!item->field) {
    items->default_eval = item->target;
    items->lowest = 0;
  } else {
    Array_append(items->itemArray, item, 1);
    AVLTree_set(items->itemTree, (uint64_t) item->field, (void *) 1);
    items->lowest = item->field->lower;
  }
  allocator->free(item);
  return items;
}

MemItem *p_MemItem_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];

  grammarAssertNotDeclaredMemItem(name);
  grammarAssert(width <= 64, "too long to support this width.");

  MemItem *item = allocator->calloc(1, sizeof(MemItem));
  item->name = name;
  item->width = width;
  item->type = nullptr;

  return item;
}

MemItem *p_MemItem_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Identifier *type = (Identifier *) argv[4];

  grammarAssertNotDeclaredMemItem(name);
  grammarAssertDefinedRecord(type);
  grammarAssert(width <= 64, "too long to support this width.");
  const Record *record = GContext_findRecord(context, type);
  if (record->typeid == enum_Immediate) {
    const Immediate *imm = GContext_getImmediate(context, record->offset);
    grammarAssert(imm->width == width, "incompatible width.");
  }

  MemItem *item = allocator->calloc(1, sizeof(MemItem));
  item->name = name;
  item->width = width;
  item->type = type;

  return item;
}

MemItems *p_MemItems_0(void *argv[], GContext *, const Allocator *allocator) {
  MemItems *items = (MemItems *) argv[0];
  MemItem *item = (MemItem *) argv[1];

  MemItem *last_item = Array_last_real(items);
  item->start = last_item->start + last_item->width;

  Array_append(items, item, 1);
  allocator->free(item);

  return items;
}

MemItems *p_MemItems_1(void *argv[], GContext *context, const Allocator *allocator) {
  MemItem *item = (MemItem *) argv[0];

  item->start = 0;

  MemItems *items = Array_new(sizeof(MemItem), enum_MemItem, allocator);
  Array_append(items, item, 1);
  allocator->free(item);

  Gcontext_setItems(context, items);
  return items;
}

Memory *p_Memory_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  MemItems *items = (MemItems *) argv[4];

  grammarAssert(width <= 64, "too long to support this width.");

  Memory mem = {.name = ident, .width = width, .items = items};

  REFER(Memory) result = GContext_addMemory(context, &mem);

  return result;
}

Parameter *p_Parameter_0(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *type = (Identifier *) argv[0];
  Identifier *name = (Identifier *) argv[1];

  grammarAssertDefinedRecord(type);
  grammarAssertNotDeclaredRecord(name);

  Parameter *param = allocator->calloc(1, sizeof(Parameter));
  param->type = type;
  param->name = name;

  return param;
}

Pattern *p_Pattern_0(void *argv[], GContext *context, const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[1];

  if (GContext_testPattern(context, args)) {
    GContext_setErrorMessage(context, "duplicated instruction pattern.");
    return nullptr;
  }

  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = args;

  GContext_addPattern(context, pattern);

  return pattern;
}

Pattern *p_Pattern_1(void *[], GContext *context, const Allocator *allocator) {
  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = nullptr;

  GContext_addPattern(context, pattern);

  return pattern;
}

PatternArgs *p_PatternArgs_0(void *argv[], GContext *, const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[0];
  Parameter *param = (Parameter *) argv[2];

  Array_append(args, param, 1);
  allocator->free(param);
  return args;
}

PatternArgs *p_PatternArgs_1(void *argv[], GContext *, const Allocator *allocator) {
  Parameter *param = (Parameter *) argv[0];

  PatternArgs *args = Array_new(sizeof(Parameter), enum_PatternArgs, allocator);
  Array_append(args, param, 1);
  allocator->free(param);

  return args;
}

Register *p_Register_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[0];
  BitField *field = (BitField *) argv[2];
  uint64_t code = (uint64_t) argv[4];

  grammarAssertNotDeclaredRecord(ident);

  Register reg = {.name = ident, .field = field, .code = code};
  return GContext_addRegister(context, &reg);
}

RegisterGroup *p_RegisterGroup_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1];
  uint32_t width = (uint32_t) (uint64_t) argv[2];
  Registers *registers = (Registers *) argv[4];

  grammarAssertNotDeclaredRecord(ident);

  const uint32_t len = Array_length(registers);
  grammarAssert(len > 0, "no register defined.");

  RegisterGroup grp = {.name = ident, .width = width, .registers = registers};
  REFER(RegisterGroup) result = GContext_addRegisterGroup(context, &grp);
  REFER(Register) *regs = Array_real_addr(registers, 0);
  for (uint32_t i = 0; i < len; i++) {
    Register *reg = Array_vert2real(context->regArray, regs[i]);
    reg->group = result;
  }

  return result;
}

Registers *p_Registers_0(void *argv[], GContext *, const Allocator *) {
  Registers *regs = (Registers *) argv[0];
  Register *reg = (Register *) argv[1];
  Array_append(regs, &reg, 1);
  return regs;
}

Registers *p_Registers_1(void *argv[], GContext *, const Allocator *allocator) {
  Register *reg = argv[0];
  Registers *regs = Array_new(sizeof(REFER(Register)), -1, allocator);
  Array_append(regs, &reg, 1);
  return regs;
}

Set *p_Set_0(void *argv[], GContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1];
  SetItems *items = (SetItems *) argv[3];

  grammarAssertNotDeclaredRecord(ident);

  Set set = {.name = ident, .items = items};

  return GContext_addSet(context, &set);
}

SetItems *p_SetItems_0(void *argv[], GContext *context, const Allocator *allocator) {
  SetItems *items = (SetItems *) argv[0];
  Identifier *ident = (Identifier *) argv[2];

  grammarAssertDefinedRecord(ident);

  Array_append(items, ident, 1);
  allocator->free(ident);

  return items;
}

SetItems *p_SetItems_1(void *argv[], GContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0];

  grammarAssertDefinedRecord(ident);

  SetItems *items = Array_new(sizeof(Identifier), enum_IDENTIFIER, allocator);
  Array_append(items, ident, 1);
  allocator->free(ident);

  return items;
}

#include "stack.h"

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *token, uint32_t type, const Allocator *allocator
);

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *args[], int32_t *states, uint32_t argc,
    const Allocator *allocator
);

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *token, uint32_t type, const Allocator *allocator
) {
  int32_t state = 0;
  Stack_top(state_stack, (&state), sizeof(int32_t));
  releaseToken(token, type, allocator);
  allocator->free(token);
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *args[], int32_t *states, uint32_t argc,
    const Allocator *allocator
) {
  for (uint32_t i = 0; i < argc; i++) {
    uint32_t type = getParseStateCurrentTokenType(states[i]);
    releaseToken(args[i], type, allocator);
  }
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator) {
  void *token = nullptr;
  int32_t state = -1;
  while (!Stack_empty(token_stack)) {
    Stack_pop(token_stack, &token, sizeof(void *));
    Stack_pop(state_stack, &state, sizeof(int32_t));
    uint32_t type = getParseStateCurrentTokenType(state);
    releaseToken(token, type, allocator);
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  return nullptr;
}

#define releaseArrayCase(array, ele)                 \
  case enum_##array: {                               \
    Array_reset(token, (destruct_t *) release##ele); \
    Array_destroy(token);                            \
    break;                                           \
  }
#define releaseTokenCase(t, r)    \
  case enum_##t: {                \
    release##r(token, allocator); \
    break;                        \
  }

void releaseToken(void *token, uint32_t type, const Allocator *allocator) {
  switch (type) {
    //    releaseArrayCase(Entries, Entry)
    releaseArrayCase(InstrForms, InstrForm)
    releaseArrayCase(InstrParts, InstrPart)
    releaseArrayCase(PatternArgs, Identifier)
    releaseArrayCase(Registers, Register)
    releaseArrayCase(SetItems, Identifier)

    releaseTokenCase(Entry, Entry)
    releaseTokenCase(Condition, Condition)
    releaseTokenCase(Variable , Variable)
    releaseTokenCase(Evaluable, Evaluable)
    releaseTokenCase(Immediate, Immediate)
    releaseTokenCase(InstrForm, InstrForm)
    releaseTokenCase(InstrPart, InstrPart)
    releaseTokenCase(Instruction, Instruction)
    releaseTokenCase(Layout, Layout)
    releaseTokenCase(Machine, Machine)
    releaseTokenCase(MappingItem, MappingItem)
    releaseTokenCase(MappingItems, MappingItems)
    releaseTokenCase(MemItem, MemItem)
    releaseTokenCase(Memory, Memory)
    releaseTokenCase(Pattern, Pattern)
    releaseTokenCase(Register, Register)
    releaseTokenCase(RegisterGroup, RegisterGroup)
    releaseTokenCase(Set, Set)
    case enum_CondExpr:
    case enum_AndCondExpr:
    case enum_SingleCondExpr:
    case enum_Arith_0_Expr:
    case enum_Arith_1_Expr:
    case enum_Arith_2_Expr:
    case enum_Arith_3_Expr: {
      releaseExpr(token, allocator);
      allocator->free(token);
      break;
    }
    case enum_IDENTIFIER: {
      releaseIdentifier(token, allocator);
      allocator->free(token);
      break;
    }
    case enum_BIT_FIELD: {
      releaseBitField(token, allocator);
      allocator->free(token);
      break;
    }
    default: {
    }
  }
}
