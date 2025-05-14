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
 * Filename: terminal.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TERMINAL_H
#define MACHINE_TERMINAL_H

#include "allocator.h"
#include "char_t.h"
#include "generated/tokens.gen.h"
#include <stdint.h>

typedef struct {
  uint32_t lineno;
  uint32_t column;
} TokenPos[2];

typedef struct Token {
  void *value;
  enum TOKEN_TYPE_ENUM type;
  uint32_t length;
  TokenPos position;
} Token;

typedef struct Terminal {
  void *value;
  uint32_t type;
  uint32_t length;
  uint32_t lineno;
  uint32_t column;
} Terminal;

typedef char_t Identifier;

typedef struct BitField {
  uint32_t upper;
  uint32_t lower;
} BitField;

int32_t BitField_cmp(void *bf1, void *bf2);
int32_t Identifier_cmp(void *a, void *b);

void terminal_to_token(Token *token, const Terminal *tp);
uint64_t get_char(const void *key);

#define BitField_toUint64(p_bf) ((((uint64_t) ((p_bf)->upper)) << 32) | ((p_bf)->lower))
#define BitField_getUpper(bf_uint64) ((uint32_t)((bf_uint64) >> 32))
#define BitField_getLower(bf_uint64) ((uint32_t)((bf_uint64) & 0xFFFFFFFF))
#define BitField_fromUint64(bf_uint64) { \
    .upper = BitField_getUpper(bf_uint64), \
    .lower = BitField_getLower(bf_uint64)  \
}
#endif  // MACHINE_TERMINAL_H
