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

enum IMMEDIATE_TYPE {
  IT_UNSIGNED,
  IT_SIGNED,
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
  AS_ID,
  RECU_OP_MAX = AS_ID,
  CB_IN,
  CB_LT,
  CB_LE,
  CB_GT,
  CB_GE,
  CB_EQ,
  CB_NE,
};

#endif  // MACHINE_ENUM_H
