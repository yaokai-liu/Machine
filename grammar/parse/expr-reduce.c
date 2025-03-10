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
 * Filename: optimize.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-13
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "expr-reduce.h"
#include "enum.h"
#include "generated/tokens.gen.h"
#include "target.h"

#define bool_inverse_case_item(a, b)                        \
  case a: {                                                 \
    expr->type = b;                                         \
    CondExpr *lhs = allocator->calloc(1, sizeof(CondExpr)); \
    lhs->type = enum_BOOL_NOT;                              \
    lhs->lhs = nullptr;                                     \
    lhs->rhs = rhs->lhs;                                    \
    rhs->type = enum_BOOL_NOT;                              \
    rhs->lhs = nullptr;                                     \
    lhs = optimize_not_expr(lhs, allocator);                \
    rhs = optimize_not_expr(rhs, allocator);                \
    expr->lhs = lhs;                                        \
    expr->rhs = rhs;                                        \
    break;                                                  \
  }

#define cb_inverse_case_item(a, b) \
  case a: {                        \
    expr->type = b;                \
    expr->lhs = rhs->lhs;          \
    expr->rhs = rhs->rhs;          \
    break;                         \
  }

CondExpr *optimize_not_expr(CondExpr *expr, const Allocator *allocator) {
  SingleCondExpr *rhs = expr->rhs;
  switch (rhs->type) {
    case enum_BOOL_NOT: {
      CondExpr *r_rhs = optimize_not_expr(expr, allocator);
      expr->rhs = r_rhs->rhs;
      expr->lhs = r_rhs->lhs;
      allocator->free(rhs);
      allocator->free(r_rhs);
      break;
    }
      bool_inverse_case_item(enum_BOOL_AND, enum_BOOL_OR)
      bool_inverse_case_item(enum_BOOL_OR, enum_BOOL_AND)
      cb_inverse_case_item(CB_LT, CB_GE)
      cb_inverse_case_item(CB_LE, CB_GT)
      cb_inverse_case_item(CB_GT, CB_LE)
      cb_inverse_case_item(CB_GE, CB_LT)
      cb_inverse_case_item(CB_EQ, CB_NE)
      cb_inverse_case_item(CB_NE, CB_EQ)
    default: {
    }
  }
  return expr;
}
