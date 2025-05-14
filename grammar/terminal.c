/* License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2025 Yaokai Liu
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
 *
 * Project Name: machine
 * Module Name: grammar
 * Filename: terminal.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "terminal.h"

inline int32_t BitField_cmp(void *a, void *b) {
  uint32_t bf1_upper = BitField_getUpper((uint64_t) a);
  uint32_t bf1_lower = BitField_getLower((uint64_t) a);
  uint32_t bf2_upper = BitField_getUpper((uint64_t) b);
  uint32_t bf2_lower = BitField_getLower((uint64_t) b);
  if (bf1_upper < bf2_lower) { return -1; }
  if (bf1_lower > bf2_upper) { return 1; }
  return (bf2_upper < bf2_lower) - (bf1_upper < bf1_lower);
}

int32_t Identifier_cmp(void *a, void *b) {
  uint64_t v1 = *(uint64_t *) a;
  uint64_t v2 = *(uint64_t *) b;
  return (int32_t) (v1 & 0xFFFFFFFF) - (int32_t) (v2 & 0xFFFFFFFF);
}

inline void terminal_to_token(Token *token, const Terminal *tp) {
  token->type = tp->type;
  token->value = tp->value;
  token->length = tp->length;
  token->position[0].lineno = tp->lineno;
  token->position[0].column = tp->column;
  token->position[1].lineno = tp->lineno;
  token->position[1].column = tp->column + tp->length;
}

uint64_t get_char(const void *key) {
  return *(const char_t *) key;
}
