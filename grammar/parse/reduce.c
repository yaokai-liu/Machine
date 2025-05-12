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

#include "array.h"
#include "avl-tree.h"
#include "context.h"
#include "enum.h"
#include "expr-reduce.h"
#include "generated/tokens.gen.h"
#include "semantic.h"
#include "target.h"
#include "terminal.h"
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

Arith_0_Expr *Parse_Arith_0_Expr_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Arith_0_Expr *lhs = (Arith_0_Expr *) argv[0].value;
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1].value;
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2].value;

  Arith_0_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}

Arith_0_Expr *Parse_Arith_0_Expr_1(Token argv[], ParseContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0].value;
  Arith_0_Expr *rhs = (Arith_1_Expr *) argv[1].value;

  Arith_0_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}

Arith_0_Expr *Parse_Arith_0_Expr_2(Token argv[], ParseContext *, const Allocator *) {
  return (Arith_1_Expr *) argv[0].value;
}

Arith_1_Expr *Parse_Arith_1_Expr_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Arith_1_Expr *lhs = (Arith_1_Expr *) argv[0].value;
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1].value;
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2].value;

  Arith_1_Expr *expr = allocator->calloc(1, sizeof(Arith_1_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
Arith_1_Expr *Parse_Arith_1_Expr_1(Token argv[], ParseContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0].value;
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[1].value;

  Arith_1_Expr *expr = allocator->calloc(1, sizeof(Arith_0_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_1_Expr *Parse_Arith_1_Expr_2(Token argv[], ParseContext *, const Allocator *) {
  return (Arith_1_Expr *) argv[0].value;
}

Arith_2_Expr *Parse_Arith_2_Expr_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Arith_2_Expr *lhs = (Arith_2_Expr *) argv[0].value;
  uint32_t bin_op = (uint32_t) (uint64_t) argv[1].value;
  Arith_1_Expr *rhs = (Arith_1_Expr *) argv[2].value;

  Arith_2_Expr *expr = allocator->calloc(1, sizeof(Arith_2_Expr));
  expr->type = bin_op;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
Arith_2_Expr *Parse_Arith_2_Expr_1(Token argv[], ParseContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0].value;
  Arith_2_Expr *rhs = (Arith_1_Expr *) argv[1].value;

  Arith_2_Expr *expr = allocator->calloc(1, sizeof(Arith_2_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_2_Expr *Parse_Arith_2_Expr_2(Token argv[], ParseContext *, const Allocator *) {
  return (Arith_2_Expr *) argv[0].value;
}

Arith_3_Expr *Parse_Arith_3_Expr_0(Token argv[], ParseContext *, const Allocator *) {
  return (Arith_3_Expr *) argv[1].value;
}

Arith_3_Expr *Parse_Arith_3_Expr_1(Token argv[], ParseContext *, const Allocator *allocator) {
  uint32_t sin_op = (uint32_t) (uint64_t) argv[0].value;
  Arith_3_Expr *rhs = (Arith_1_Expr *) argv[1].value;

  Arith_3_Expr *expr = allocator->calloc(1, sizeof(Arith_3_Expr));
  expr->type = sin_op;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
Arith_3_Expr *Parse_Arith_3_Expr_2(Token argv[], ParseContext *, const Allocator *allocator) {
  Evaluable *eval = (Evaluable *) argv[0].value;
  Arith_3_Expr *expr = allocator->calloc(1, sizeof(Arith_3_Expr));
  expr->type = AS_ID;
  expr->lhs = nullptr;
  expr->rhs = eval;
  return expr;
}

Condition *Parse_Condition_0(Token argv[], ParseContext *, const Allocator *allocator) {
  CondExpr *expr = (CondExpr *) argv[2].value;
  Condition *cond = allocator->calloc(1, sizeof(Condition));
  cond->expr = expr;
  return cond;
}
CondExpr *Parse_CondExpr_0(Token argv[], ParseContext *, const Allocator *allocator) {
  CondExpr *lhs = (CondExpr *) argv[0].value;
  AndCondExpr *rhs = (AndCondExpr *) argv[2].value;

  CondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_OR;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}

CondExpr *Parse_CondExpr_1(Token argv[], ParseContext *, const Allocator *) {
  return (CondExpr *) argv[0].value;
}

AndCondExpr *Parse_AndCondExpr_0(Token argv[], ParseContext *, const Allocator *allocator) {
  AndCondExpr *lhs = (AndCondExpr *) argv[0].value;
  SingleCondExpr *rhs = (SingleCondExpr *) argv[2].value;

  AndCondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_AND;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
AndCondExpr *Parse_AndCondExpr_1(Token argv[], ParseContext *, const Allocator *) {
  return (CondExpr *) argv[0].value;
}
SingleCondExpr *Parse_SingleCondExpr_0(Token argv[], ParseContext *, const Allocator *) {
  return (CondExpr *) argv[1].value;
}
CondExpr *Parse_SingleCondExpr_1(Token argv[], ParseContext *, const Allocator *allocator) {
  SingleCondExpr *rhs = (SingleCondExpr *) argv[1].value;

  CondExpr *expr = allocator->calloc(1, sizeof(CondExpr));
  expr->type = enum_BOOL_NOT;
  expr->lhs = nullptr;
  expr->rhs = rhs;

  expr = optimize_not_expr(expr, allocator);

  return expr;
}
SingleCondExpr *Parse_SingleCondExpr_2(Token argv[], ParseContext *, const Allocator *allocator) {
  SingleCondExpr *lhs = (SingleCondExpr *) argv[0].value;
  uint32_t type = (uint32_t) (uint64_t) argv[1].value;
  SingleCondExpr *rhs = (SingleCondExpr *) argv[2].value;

  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = type;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *
    Parse_SingleCondExpr_3(Token argv[], ParseContext *context, const Allocator *allocator) {
  Variable *lhs = (Variable *) argv[0].value;
  Identifier *rhs = (Identifier *) argv[2].value;

  grammarAssertDefinedRecord(rhs);
  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = CB_IN;
  expr->lhs = lhs;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *Parse_SingleCondExpr_4(Token argv[], ParseContext *, const Allocator *allocator) {
  uint32_t type = (uint32_t) (uint64_t) argv[0].value;
  Arith_0_Expr *rhs = (Arith_0_Expr *) argv[1].value;

  SingleCondExpr *expr = allocator->calloc(1, sizeof(SingleCondExpr));
  expr->type = type;
  expr->lhs = nullptr;
  expr->rhs = rhs;
  return expr;
}
SingleCondExpr *Parse_SingleCondExpr_5(Token argv[], ParseContext *, const Allocator *) {
  return (Arith_0_Expr *) argv[0].value;
}

Options *Parse_Options_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Options *options = (Options *) argv[0].value;
  Arith_0_Expr *evaluable = (Arith_0_Expr *) argv[2].value;

  Array_append(options, evaluable, 1);
  allocator->free(evaluable);
  return options;
}

Options *Parse_Options_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Arith_0_Expr *evaluable = (Arith_0_Expr *) argv[0].value;

  Options *options = Array_new(sizeof(Arith_0_Expr), enum_Arith_0_Expr, allocator);
  Array_append(options, evaluable, 1);
  allocator->free(evaluable);
  return options;
}

Switchable *Parse_Switchable_0(Token argv[], ParseContext *, const Allocator *allocator) {
  CondExpr *expr = (CondExpr *) argv[1].value;
  Options *options = (Options *) argv[4].value;

  Switchable *switchable = allocator->calloc(1, sizeof(Switchable));
  switchable->expr = expr;
  switchable->options = options;
  return switchable;
}

GrammarEntries *Parse_GrammarEntries_0(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntries))(uint64_t) (enum_GrammarEntries);
}

GrammarEntries *Parse_GrammarEntries_1(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntries))(uint64_t) (enum_GrammarEntries);
}

GrammarEntry *Parse_GrammarEntry_0(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

GrammarEntry *Parse_GrammarEntry_1(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

GrammarEntry *Parse_GrammarEntry_2(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

GrammarEntry *Parse_GrammarEntry_3(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

GrammarEntry *Parse_GrammarEntry_4(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

GrammarEntry *Parse_GrammarEntry_5(Token[], ParseContext *, const Allocator *) {
  return (REFER(GrammarEntry))(uint64_t) (enum_GrammarEntry);
}

Variable *Parse_Variable_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *lhs = (Identifier *) argv[0].value;
  Identifier *rhs = (Identifier *) argv[2].value;

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
  Variable *var = allocator->calloc(1, sizeof(Variable));
  var->type = enum_MemItem;
  var->lhs = lhs;
  var->rhs = Array_virt2real(memory->items, item);
  return var;
}

Variable *Parse_Variable_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0].value;

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

Evaluable *Parse_Evaluable_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Variable *lhs = (Variable *) argv[0].value;
  BitField *rhs = (BitField *) &argv[1].value;

  const Record *record = nullptr;
  Pattern *pattern = *(Pattern **) Array_last_real(GContext_getPatternArray(context));
  Parameter *first = Array_first_real(pattern->args);
  Parameter *last = Array_last_real(pattern->args);
  for (Parameter *param = first; param <= last; param++) {
    if (param->name == lhs->lhs) { record = GContext_findRecord(context, param->type); }
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
      break;
    }
  }

  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_BIT_FIELD;
  evaluable->lhs = lhs;
  evaluable->rhs = *(void **) rhs;
  return evaluable;
}

Evaluable *Parse_Evaluable_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Variable *var = (Variable *) argv[1].value;
  uint32_t width = 0;
  switch (var->type) {
    case VT_IDENTIFIER: {
      Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
      evaluable->type = enum_OP_WIDTH;
      evaluable->lhs = var;
      evaluable->rhs = nullptr;
      return evaluable;
    }
    case VT_REGISTER: {
      const Register *reg = var->rhs;
      width = reg->field.upper - reg->field.lower + 1;
      break;
    }
    case VT_MEMORY: {
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
    default: {
      return nullptr;
    }
  }
  allocator->free(var);
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) (uint64_t) width;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *Parse_Evaluable_2(Token argv[], ParseContext *context, const Allocator *allocator) {
  Variable *var = (Variable *) argv[0].value;

  if (var->type == VT_REGISTER) {
    const Register *reg = var->rhs;
    allocator->free(var);
    Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
    evaluable->type = enum_NUMBER;
    evaluable->lhs = (void *) reg->code;
    evaluable->rhs = nullptr;
    return evaluable;
  }
  grammarAssert(
      var->type != VT_MEMORY && var->type != VT_IMMEDIATE,
      "arithmetic operation with 'Memory' or 'Immediate' entity is not supported."
  );
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_Variable;
  evaluable->lhs = var;
  evaluable->rhs = nullptr;
  return evaluable;
}

Evaluable *Parse_Evaluable_3(Token argv[], ParseContext *, const Allocator *allocator) {
  uint64_t number = (uint64_t) argv[0].value;
  Evaluable *evaluable = allocator->calloc(1, sizeof(Evaluable));
  evaluable->type = enum_NUMBER;
  evaluable->lhs = (void *) number;
  evaluable->rhs = nullptr;
  return evaluable;
}

Immediate *Parse_Immediate_0(Token argv[], ParseContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  uint32_t type = (uint64_t) argv[3].value;

  grammarAssertNotDeclaredRecord(ident);
  grammarAssert(width <= 64, "too long to support this width.");

  Immediate imm = {.type = type, .width = width, .name = ident};

  Immediate *result = GContext_addImmediate(context, &imm);

  return result;
}

InstrForm *Parse_InstrForm_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0].value;
  FormCheck *check = (FormCheck *) argv[3].value;
  InstrParts *part_array = (InstrParts *) argv[4].value;
  uint32_t width = 0;
  const InstrPart *first = Array_first_real(part_array);
  const InstrPart *last = Array_last_real(part_array);
  for (const InstrPart *part = first; part <= last; part++) {
    width += part->width > 256 ? 256 : part->width;
  }
  if (pattern->args) {
    const uint32_t n_args = Array_length(pattern->args);
    const Parameter *args = Array_real_addr(pattern->args, 0);
    for (uint32_t i = 0; i < n_args; i++) {
      if (args[i].used) { pattern->used_args++; }
    }
  }

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = 1;
  form->pattern = pattern;
  form->check = (check == (void *) enum_FormCheck) ? nullptr : check;
  form->parts = part_array;

  return form;
}

InstrForm *Parse_InstrForm_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Pattern *pattern = (Pattern *) argv[0].value;
  uint32_t tick = (uint32_t) (uint64_t) argv[3].value;
  FormCheck *check = (FormCheck *) argv[4].value;
  InstrParts *part_array = (InstrParts *) argv[5].value;

  uint32_t width = 0;
  const InstrPart *first = Array_first_real(part_array);
  const InstrPart *last = Array_last_real(part_array);
  for (const InstrPart *part = first; part <= last; part++) {
    width += part->width > 256 ? 256 : part->width;
  }
  if (pattern->args) {
    const uint32_t n_args = Array_length(pattern->args);
    const Parameter *args = Array_real_addr(pattern->args, 0);
    for (uint32_t i = 0; i < n_args; i++) {
      if (args[i].used) { pattern->used_args++; }
    }
  }

  InstrForm *form = allocator->calloc(1, sizeof(InstrForm));
  form->width = width;
  form->tick = tick;
  form->pattern = pattern;
  form->check = (check == (void *) enum_FormCheck) ? nullptr : check;
  form->parts = part_array;
  return form;
}

InstrForms *Parse_InstrForms_0(Token argv[], ParseContext *, const Allocator *allocator) {
  InstrForms *forms = (InstrForms *) argv[0].value;
  InstrForm *form = (InstrForm *) argv[1].value;
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

InstrForms *Parse_InstrForms_1(Token argv[], ParseContext *, const Allocator *allocator) {
  InstrForm *form = (InstrForm *) argv[0].value;
  InstrForms *forms = Array_new(sizeof(InstrForm), enum_InstrForm, allocator);
  Array_append(forms, form, 1);
  allocator->free(form);
  return forms;
}

FormCheck *Parse_FormCheck_0(Token argv[], ParseContext *, const Allocator *) {
  FormCheck *check = (FormCheck *) argv[0].value;
  return check;
}

FormCheck *Parse_FormCheck_1(Token[], ParseContext *, const Allocator *) {
  return (FormCheck *) (uint64_t) enum_FormCheck;
}

InstrPart *Parse_InstrPart_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  Layout *layout = (Layout *) argv[4].value;

  grammarAssert(width != 0, "empty part.");

  if (width == (uint32_t) -1) {
    const Arith_0_Expr *expr = layout->target;
    grammarAssert(
        layout->type == enum_Arith_0_Expr && expr->type == AS_ID,
        "target is too complex to calculate width."
    );
  } else {
    grammarAssert(
        width % 8 == 0, "illegal width of parts. width must be an integer multiple of a byte with."
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

InstrPart *Parse_InstrPart_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  Layout *layout = (Layout *) argv[4].value;
  Condition *condition = (Condition *) argv[5].value;

  grammarAssert(width != 0, "empty part.");

  if (width == (uint32_t) -1) {
    const Arith_0_Expr *expr = layout->target;
    grammarAssert(
        layout->type == enum_Arith_0_Expr && expr->type == AS_ID,
        "target is too complex to calculate width."
    );
  } else {
    grammarAssert(
        width % 8 == 0, "illegal width of parts. width must be an integer multiple of a byte with."
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

InstrParts *Parse_InstrParts_0(Token argv[], ParseContext *, const Allocator *allocator) {
  InstrParts *parts = (InstrParts *) argv[0].value;
  InstrPart *part = (InstrPart *) argv[1].value;
  Array_append(parts, part, 1);
  allocator->free(part);
  return parts;
}

InstrParts *Parse_InstrParts_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  InstrPart *part = (InstrPart *) argv[0].value;
  InstrParts *parts = Array_new(sizeof(InstrPart), enum_InstrPart, allocator);
  Array_append(parts, part, 1);
  allocator->free(part);

  Gcontext_setParts(context, parts);

  return parts;
}

Instruction *Parse_Instruction_0(Token argv[], ParseContext *context, const Allocator *) {
  Identifier *identifier = (Identifier *) argv[1].value;
  InstrForms *forms = (InstrForms *) argv[3].value;

  grammarAssertNotDeclaredOpcode(identifier);

  Instruction instruction = {.name = identifier, .forms = forms};
  REFER(Instruction) instr = GContext_addInstruction(context, &instruction);
  GContext_dump_instruction(context, instr);
  return instr;
}

Layout *Parse_Layout_0(Token argv[], ParseContext *, const Allocator *allocator) {
  Arith_0_Expr *expr = (Arith_0_Expr *) argv[0].value;
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Arith_0_Expr;
  layout->target = expr;
  return layout;
}

Layout *Parse_Layout_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Switchable *switchable = (Switchable *) argv[0].value;
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_Switchable;
  layout->target = switchable;
  return layout;
}

Layout *Parse_Layout_2(Token argv[], ParseContext *, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[1].value;
  Layout *layout = allocator->calloc(1, sizeof(Layout));
  layout->type = enum_MappingItems;
  layout->target = items;
  return layout;
}

Machine *Parse_Machine_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *identifier = (Identifier *) argv[1].value;
  Machine *machine = allocator->calloc(1, sizeof(Machine));
  machine->name = identifier;
  machine->context = context;

  return machine;
}

Machine *Parse_Machine_EXT(Token argv[], ParseContext *, const Allocator *) {
  return (Machine *) argv[0].value;
}

MappingItem *Parse_MappingItem_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  BitField *bit_field = (BitField *) &argv[0].value;
  Arith_0_Expr *expr = (Arith_0_Expr *) argv[2].value;

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
  item->field.upper = bit_field->upper;
  item->field.lower = bit_field->lower;
  item->target = expr;

  GContext_addMapItem(context, item);

  return item;
}
MappingItem *Parse_MappingItem_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  BitField *bit_field = (BitField *) &argv[0].value;
  Switchable *switchable = (Switchable *) argv[2].value;

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
  item->field.upper = bit_field->upper;
  item->field.lower = bit_field->lower;
  item->target = switchable;

  GContext_addMapItem(context, item);

  return item;
}

MappingItems *Parse_MappingItems_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  MappingItems *items = (MappingItems *) argv[0].value;
  MappingItem *item = (MappingItem *) argv[2].value;

  if (item->field.upper < item->field.lower) {
    if (items->default_eval) {
      GContext_setErrorMessage(context, "redefine default bits.");
      return nullptr;
    }
    items->default_eval = item->target;
  } else {
    items->lowest = min(item->field.lower, items->lowest);
    uint32_t index = Array_length(items->itemArray);
    uint64_t *key = (uint64_t *) &item->field;
    AVLTree_set(items->itemTree, *key, (void *) (uint64_t) index + 1);
    Array_append(items->itemArray, item, 1);
  }
  allocator->free(item);
  return items;
}

MappingItems *Parse_MappingItems_1(Token argv[], ParseContext *, const Allocator *allocator) {
  MappingItem *item = (MappingItem *) argv[0].value;

  MappingItems *items = allocator->calloc(1, sizeof(MappingItems));
  items->itemArray = Array_new(sizeof(MappingItem), enum_MappingItem, allocator);
  items->itemTree = AVLTree_new(allocator, (compare_t *) BitField_cmp);
  if (item->field.upper < item->field.lower) {
    items->default_eval = item->target;
    items->lowest = 0;
  } else {
    Array_append(items->itemArray, item, 1);
    uint64_t *key = (uint64_t *) &item->field;
    AVLTree_set(items->itemTree, *key, (void *) 1);
    items->lowest = item->field.lower;
  }
  allocator->free(item);
  return items;
}

MemItem *Parse_MemItem_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;

  grammarAssertNotDeclaredMemItem(name);
  grammarAssert(width <= 64, "too long to support this width.");

  MemItem *item = allocator->calloc(1, sizeof(MemItem));
  item->name = name;
  item->width = width;
  item->type = nullptr;

  return item;
}

MemItem *Parse_MemItem_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *name = (Identifier *) argv[0].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  Identifier *type = (Identifier *) argv[4].value;

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

MemItems *Parse_MemItems_0(Token argv[], ParseContext *, const Allocator *allocator) {
  MemItems *items = (MemItems *) argv[0].value;
  MemItem *item = (MemItem *) argv[1].value;

  MemItem *last_item = Array_last_real(items);
  item->start = last_item->start + last_item->width;

  Array_append(items, item, 1);
  allocator->free(item);

  return items;
}

MemItems *Parse_MemItems_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  MemItem *item = (MemItem *) argv[0].value;

  item->start = 0;

  MemItems *items = Array_new(sizeof(MemItem), enum_MemItem, allocator);
  Array_append(items, item, 1);
  allocator->free(item);

  Gcontext_setItems(context, items);
  return items;
}

Memory *Parse_Memory_0(Token argv[], ParseContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  MemItems *items = (MemItems *) argv[4].value;

  grammarAssert(width <= 64, "too long to support this width.");

  Memory mem = {.name = ident, .width = width, .items = items};

  REFER(Memory) result = GContext_addMemory(context, &mem);

  return result;
}

Parameter *Parse_Parameter_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *type = (Identifier *) argv[0].value;
  Identifier *name = (Identifier *) argv[1].value;

  grammarAssertDefinedRecord(type);
  grammarAssertNotDeclaredRecord(name);

  Parameter *param = allocator->calloc(1, sizeof(Parameter));
  param->type = type;
  param->name = name;

  return param;
}

Pattern *Parse_Pattern_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  PatternArgs *arg_array = (PatternArgs *) argv[1].value;

  if (GContext_testPattern(context, arg_array)) {
    GContext_setErrorMessage(context, "duplicated instruction pattern.");
    return nullptr;
  }

  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = arg_array;

  GContext_addPattern(context, pattern);

  return pattern;
}

Pattern *Parse_Pattern_1(Token[], ParseContext *context, const Allocator *allocator) {
  Pattern *pattern = allocator->calloc(1, sizeof(Pattern));
  pattern->args = nullptr;

  GContext_addPattern(context, pattern);

  return pattern;
}

PatternArgs *Parse_PatternArgs_0(Token argv[], ParseContext *, const Allocator *allocator) {
  PatternArgs *args = (PatternArgs *) argv[0].value;
  Parameter *param = (Parameter *) argv[2].value;

  Array_append(args, param, 1);
  allocator->free(param);
  return args;
}

PatternArgs *Parse_PatternArgs_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Parameter *param = (Parameter *) argv[0].value;

  PatternArgs *args = Array_new(sizeof(Parameter), enum_PatternArgs, allocator);
  Array_append(args, param, 1);
  allocator->free(param);

  return args;
}

Register *Parse_Register_0(Token argv[], ParseContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[0].value;
  BitField *field = (BitField *) &argv[2].value;
  uint64_t code = (uint64_t) argv[4].value;

  grammarAssertNotDeclaredRecord(ident);

  Register reg = {
      .name = ident, .field = {.upper = field->upper, .lower = field->lower},
           .code = code
  };
  return GContext_addRegister(context, &reg);
}

RegisterGroup *Parse_RegisterGroup_0(Token argv[], ParseContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1].value;
  uint32_t width = (uint32_t) (uint64_t) argv[2].value;
  Registers *registers = (Registers *) argv[4].value;

  grammarAssertNotDeclaredRecord(ident);

  const uint32_t len = Array_length(registers);
  grammarAssert(len > 0, "no register defined.");

  RegisterGroup grp = {.name = ident, .width = width, .registers = registers};
  REFER(RegisterGroup) result = GContext_addRegisterGroup(context, &grp);
  REFER(Register) *regs = Array_real_addr(registers, 0);
  for (uint32_t i = 0; i < len; i++) {
    Register *reg = GContext_referToRegister(context, regs[i]);
    reg->group = result;
  }

  return result;
}

Registers *Parse_Registers_0(Token argv[], ParseContext *, const Allocator *) {
  Registers *regs = (Registers *) argv[0].value;
  Register *reg = (Register *) argv[1].value;
  Array_append(regs, &reg, 1);
  return regs;
}

Registers *Parse_Registers_1(Token argv[], ParseContext *, const Allocator *allocator) {
  Register *reg = (Register *) argv[0].value;
  Registers *regs = Array_new(sizeof(REFER(Register)), -1, allocator);
  Array_append(regs, &reg, 1);
  return regs;
}

RecordSet *Parse_RecordSet_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[1].value;
  SetExpr *expr = (SetExpr *) argv[2].value;

  grammarAssertNotDeclaredRecord(ident);

  SetItems *items = expr->lhs;

  RecordSet set = {.name = ident, .items = items};
  allocator->free(expr);

  return GContext_addRecordSet(context, &set);
}

List *Parse_List_0(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[1].value;
  SetExpr *expr = (SetExpr *) argv[2].value;

  grammarAssertNotDeclaredRecord(ident);

  SetItems *items = expr->lhs;

  List list = {.name = ident, .items = items, .width = -1};
  allocator->free(expr);

  return GContext_addList(context, &list);
}

List *Parse_List_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[1].value;
  uint64_t width = (uint64_t) argv[2].value;
  SetExpr *expr = (SetExpr *) argv[3].value;

  grammarAssertNotDeclaredRecord(ident);

  SetItems *items = expr->lhs;

  List list = {.name = ident, .items = items, .width = width};
  allocator->free(expr);

  return GContext_addList(context, &list);
}

SetItems *Parse_SetItems_0(Token argv[], ParseContext *context, const Allocator *) {
  SetItems *items = (SetItems *) argv[0].value;
  Identifier *ident = (Identifier *) argv[2].value;

  const Record *record = GContext_findRecord(context, ident);
  if (!record) {
    GContext_setErrorMessage(context, "undefined identifier.");
    return nullptr;
  }
  switch (record->typeid) {
    case enum_RegisterGroup: {
      const RegisterGroup *grp = GContext_getRegisterGroup(context, record->offset);
      Array *reg_array = grp->registers;
      const Register *regs = Array_first_real(reg_array);
      const uint32_t count = Array_length(reg_array);
      for (uint32_t i = 0; i < count; i++) { Set_add(items, regs->name); }
      break;
    }
    case enum_RecordSet: {
      const RecordSet *set = GContext_getRecordSet(context, record->offset);
      Set_update(items, set->items);
      break;
    }
    default: {
      Set_add(items, ident);
    }
  }
  return items;
}

SetItems *Parse_SetItems_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0].value;
  const Record *record = GContext_findRecord(context, ident);
  if (!record) {
    GContext_setErrorMessage(context, "undefined identifier.");
    return nullptr;
  }
  SetItems *items = Set_new(enum_IDENTIFIER, allocator, nullptr);
  switch (record->typeid) {
    case enum_RegisterGroup: {
      const RegisterGroup *grp = GContext_getRegisterGroup(context, record->offset);
      Array *reg_array = grp->registers;
      const Register *regs = Array_first_real(reg_array);
      const uint32_t count = Array_length(reg_array);
      for (uint32_t i = 0; i < count; i++) { Set_add(items, regs->name); }
      break;
    }
    case enum_RecordSet: {
      const RecordSet *set = GContext_getRecordSet(context, record->offset);
      Set_update(items, set->items);
      break;
    }
    default: {
      Set_add(items, ident);
    }
  }
  return items;
}

#define record_translate_to_set(record)                                                \
  do {                                                                                 \
    switch (record->typeid) {                                                          \
      case enum_RecordSet: {                                                           \
        const RecordSet *set = GContext_getRecordSet(context, record->offset);         \
        Set_update(items, set->items);                                                 \
        break;                                                                         \
      }                                                                                \
      case enum_RegisterGroup: {                                                       \
        const RegisterGroup *grp = GContext_getRegisterGroup(context, record->offset); \
        const Register *regs = Array_first_real(grp->registers);                       \
        const uint32_t count = Array_length(grp->registers);                           \
        for (uint32_t i = 0; i < count; i++) { Set_add(items, regs[i].name); }         \
        break;                                                                         \
      }                                                                                \
      case enum_Register: {                                                            \
        const Register *reg = GContext_getRegister(context, record->offset);           \
        Set_add(items, reg->name);                                                     \
        break;                                                                         \
      }                                                                                \
      default: {                                                                       \
        GContext_setErrorMessage(context, "bad record type to translate to set.");     \
        return nullptr;                                                                \
      }                                                                                \
    }                                                                                  \
  } while (false)

SetExpr *Parse_SetExpr_0(Token argv[], ParseContext *, const Allocator *) {
  return (SetExpr *) argv[1].value;
}

SetExpr *Parse_SetExpr_1(Token argv[], ParseContext *context, const Allocator *allocator) {
  SetExpr *expr = (SetExpr *) argv[0].value;
  enum ENUM_OP optype = (uint64_t) argv[1].value;
  Identifier *ident = (Identifier *) argv[2].value;

  const Record *record = GContext_findRecord(context, ident);
  if (!record) {
    GContext_setErrorMessage(context, "undefined identifier.");
    return nullptr;
  }
  SetItems *items = Set_new(enum_IDENTIFIER, allocator, nullptr);
  record_translate_to_set(record);

  switch (optype) {
    case AB_AND: {
      Set_limit(expr->lhs, items);
      break;
    }
    case AB_XOR: {
      Set_reduce(expr->lhs, items);
      break;
    }
    case AB_OR: {
      Set_update(expr->lhs, items);
      break;
    }
    default: {
      GContext_setErrorMessage(context, "unknown operation between sets.");
      return nullptr;
    }
  }
  Set_destroy(items);

  return expr;
}

SetExpr *Parse_SetExpr_2(Token argv[], ParseContext *context, const Allocator *) {
  SetExpr *expr = (SetExpr *) argv[0].value;
  enum ENUM_OP optype = (uint64_t) argv[1].value;
  SetItems *items = (SetItems *) argv[3].value;

  switch (optype) {
    case AB_AND: {
      Set_limit(expr->lhs, items);
      break;
    }
    case AB_XOR: {
      Set_reduce(expr->lhs, items);
      break;
    }
    case AB_OR: {
      Set_update(expr->lhs, items);
      break;
    }
    default: {
      GContext_setErrorMessage(context, "unknown operation between sets.");
      return nullptr;
    }
  }
  Set_destroy(items);

  return expr;
}

SetExpr *Parse_SetExpr_3(Token argv[], ParseContext *, const Allocator *allocator) {
  SetItems *items = (SetItems *) argv[1].value;
  SetExpr *expr = allocator->calloc(1, sizeof(SetExpr));
  expr->type = AS_ID;
  expr->lhs = items;
  return expr;
}

SetExpr *Parse_SetExpr_4(Token argv[], ParseContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0].value;

  const Record *record = GContext_findRecord(context, ident);
  if (!record) {
    GContext_setErrorMessage(context, "undefined identifier.");
    return nullptr;
  }
  SetItems *items = Set_new(enum_IDENTIFIER, allocator, nullptr);
  record_translate_to_set(record);

  SetExpr *expr = allocator->calloc(1, sizeof(SetExpr));
  expr->type = AS_ID;
  expr->lhs = items;
  return expr;
}

#include "parse.h"
#include "stack.h"

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, Token *token, const Allocator *allocator
) {
  int32_t state = 0;
  Stack_top(state_stack, (&state), sizeof(int32_t));
  releaseToken(token, allocator);
  allocator->free(token);
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, Token args[], uint32_t argc, const Allocator *allocator
) {
  for (uint32_t i = 0; i < argc; i++) { releaseToken(&args[i], allocator); }
  return clean_parse_stack(state_stack, token_stack, allocator);
}

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator) {
  Token token = {};
  while (!Stack_empty(token_stack)) {
    Stack_pop(token_stack, &token, sizeof(Token));
    releaseToken(&token, allocator);
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  return nullptr;
}

#define releaseArrayCase(array, ele)                        \
  case enum_##array: {                                      \
    Array_reset(token->value, (destruct_t *) release##ele); \
    Array_destroy(token->value);                            \
    break;                                                  \
  }
#define releaseTokenCase(t, r)           \
  case enum_##t: {                       \
    release##r(token->value, allocator); \
    break;                               \
  }

void releaseToken(Token *token, const Allocator *allocator) {
  switch (token->type) {
    releaseArrayCase(InstrForms, InstrForm)
    releaseArrayCase(InstrParts, InstrPart)
    case enum_SetItems:
    case enum_Registers:
    case enum_PatternArgs: {
      releasePrimeArray(token->value);
      break;
    }

      releaseTokenCase(Condition, Condition)
      releaseTokenCase(InstrForm, InstrForm)
      releaseTokenCase(InstrPart, InstrPart)
      releaseTokenCase(Instruction, Instruction)
      releaseTokenCase(Layout, Layout)
      releaseTokenCase(Machine, Machine)
      releaseTokenCase(MappingItem, MappingItem)
      releaseTokenCase(MappingItems, MappingItems)
      releaseTokenCase(Memory, Memory)
      releaseTokenCase(Pattern, Pattern)
      releaseTokenCase(RegisterGroup, RegisterGroup)
      releaseTokenCase(RecordSet, RecordSet)
    case enum_Variable:
    case enum_CondExpr:
    case enum_Evaluable:
    case enum_AndCondExpr:
    case enum_SingleCondExpr:
    case enum_Arith_0_Expr:
    case enum_Arith_1_Expr:
    case enum_Arith_2_Expr:
    case enum_Arith_3_Expr: {
      releaseExpr(token->value, allocator);
      allocator->free(token->value);
      break;
    }
    default: {
    }
  }
}
