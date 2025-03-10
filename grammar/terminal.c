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
#include "string_t.h"

inline int32_t Identifier_cmp(const Identifier *ident1, const Identifier *ident2) {
  if (ident1 == ident2) { return 0; }
  if (!ident1) { return 1; }
  if (!ident2) { return -1; }
  if (ident1->len < ident2->len) { return -1; }
  if (ident1->len > ident2->len) { return 1; }
  uint32_t cmp_len = strcmp_o(ident1->ptr, ident2->ptr);
  return (int32_t) (ident1->len < cmp_len) ? -1 : (ident1->len > cmp_len) ? 1 : 0;
}

inline int32_t BitField_cmp(BitField *bf1, BitField *bf2) {
  if (bf1 == bf2) { return 0; }
  if (!bf1) { return -1; }
  if (!bf2) { return 1; }
  if (bf1->upper < bf2->lower) { return -1; }
  if (bf1->lower > bf2->upper) { return 1; }
  return 0;
}

inline void releaseIdentifier(Identifier *ident, const Allocator *allocator) {
  allocator->free(ident->ptr);
}

inline void releaseBitField(BitField *, const Allocator *) {}

uint64_t get_char(const void *key) {
  return *(const char_t *) key;
}
