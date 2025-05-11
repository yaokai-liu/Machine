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

#ifndef MACHINE_TOKENIZE_TOKENIZER_H
#define MACHINE_TOKENIZE_TOKENIZER_H

#include "array.h"
#include "char_t.h"
#include "context.h"
#include "err.h"
#include "terminal.h"

typedef struct Tokenizer {
  const Allocator *allocator;
  const char_t *src;
  uint32_t cost;
  uint32_t lineno;
  uint32_t column;
  MacroContext *context;

  Array *ident_array;  // Array<Identifier>
  Trie *ident_trie;    // Trie<char_t, Identifier>
  Stack *framestack;   // Stack<MacroCallFrame>
  MacroCallFrame frame;
} Tokenizer;

Tokenizer *Tokenizer_new(const char_t *src, Array *ident_array, const Allocator *allocator);
void Tokenizer_destroy(Tokenizer *tokenizer);
uint32_t Tokenizer_next(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_macro_next(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_frame_pos_to_array(Tokenizer *tokenizer, Array /*<MacroCallFrame>*/ *array);

#endif  // MACHINE_TOKENIZE_TOKENIZER_H
