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
#include <stdint.h>

#include "tokens.gen.h"

typedef struct Terminal {
  uint32_t type;
  uint32_t length;
  uint32_t lineno;
  uint32_t column;
  void *value;
} Terminal;

typedef struct Identifier {
  char_t *ptr;
  uint32_t len;
} Identifier;

typedef struct BitField {
  uint32_t upper;
  uint32_t lower;
} BitField;

extern const char_t TERMINALS[];
extern const char_t *TERMINAL_STRINGS[];
extern const uint32_t TERMINAL_STRING_LENS[];
extern const int32_t N_TERMINAL;

int32_t Identifier_cmp(const Identifier *ident1, const Identifier *ident2);
int32_t BitField_cmp(BitField *bf1, BitField *bf2);

void releaseIdentifier(Identifier *ident, const Allocator *allocator);
void releaseBitField(BitField *bf, const Allocator *allocator);


uint64_t getchar(const void *key);
#endif  // MACHINE_TERMINAL_H
