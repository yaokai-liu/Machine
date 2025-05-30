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
 * Module Name: grammar
 * Filename: enum.h
 * Creator: Yaokai Liu
 * Create Date: 2024-09-09
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ENUM_H
#define MACHINE_ENUM_H

#include "generated/tokens.gen.h"

enum IMMEDIATE_TYPE {
  IT_UNSIGNED,
  IT_SIGNED,
};

enum VAR_TYPE {
  VT_IDENTIFIER = Machine_TOKEN_IDENTIFIER,
  VT_MEM_ITEM = Machine_TOKEN_MemItem,
  VT_REGISTER = Machine_TOKEN_Register,
  VT_IMMEDIATE = Machine_TOKEN_Immediate,
  VT_MEMORY = Machine_TOKEN_Memory,
};

enum ENUM_OP {
  AB_ADD = 128,
  AB_SUB,
  AB_MUL,
  AB_DIV,
  AB_MOD,
  AB_OR,
  AB_AND,
  AB_XOR,
  AB_LSH,
  AB_RSH,
  AS_INV,
  CB_LT,
  CB_LE,
  CB_GT,
  CB_GE,
  CB_EQ,
  CB_NE,
  AS_ID,
  CB_IN,
  RECURSIVE_OP_MAX = AS_ID,
};

enum COMPILER_SECTION_ENUM : uint32_t {
  COMPILER_LEX,
  COMPILER_MACRO,
  COMPILER_PARSE,
};

enum ERROR_CODE_ENUM : uint32_t {
  SUCCESS,
  ERROR_BAD_OPERAND,
  END_OF_MACRO_FRAME,
  ERROR_UNKNOWN_WIDTH,
  ERROR_NON_SET_RECORD,
  ERROR_WIDTH_MISMATCH,
  ERROR_UNKNOWN_SYMBOL,
  ERROR_WIDTH_TOO_LONG,
  ERROR_EMPTY_INSTR_PART,
  ERROR_MISALIGNED_WIDTH,
  ERROR_UNEXPECTED_TOKEN,
  ERROR_FAILED_TO_PRODUCE,
  ERROR_CONFLICT_BIT_FIELD,
  ERROR_INCOMPATIBLE_WIDTH,
  ERROR_UNDEFINED_VARIABLE,
  ERROR_NON_ACCESSIBLE_VAR,
  ERROR_INDEX_OUT_OF_RANGE,
  ERROR_MULTIPLE_DEFINE_FORM,
  ERROR_EMPTY_REGISTER_GROUP,
  ERROR_UNDEFINED_IDENTIFIER,
  ERROR_OPERATION_UNSUPPORTED,
  ERROR_MULTIPLE_DEFINE_MACRO,
  ERROR_MULTIPLE_DEFINE_ENTRY,
  ERROR_REDEFINED_DEFAULT_BITS,
  ERROR_ARGUMENT_COUNT_MISMATCH,
  ERROR_UNEXPECTED_CONCAT_TOKEN,
  ERROR_FAILED_TO_GET_NEXT_STATE,

  ERROR_WHATEVER,
};

#endif  // MACHINE_ENUM_H
