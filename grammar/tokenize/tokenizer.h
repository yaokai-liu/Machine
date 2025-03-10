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
 * Module Name: grammar/tokenize
 * Filename: tokenizer.h
 * Creator: Yaokai Liu
 * Create Date: 2025-03-08
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENIZER_H
#define MACHINE_TOKENIZER_H

#include "allocator.h"
#include "char_t.h"
#include "terminal.h"

typedef struct Tokenizer Tokenizer;

Tokenizer *Tokenizer_new(const char_t *src, const Allocator *allocator);
uint32_t Tokenizer_next(Tokenizer *tokenizer, Terminal *result);

#endif  // MACHINE_TOKENIZER_H
