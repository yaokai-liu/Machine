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
 * Filename: tokenizer.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-08
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "tokenizer.h"
#include "action.h"
#include "array.h"
#include "context.h"
#include "generated/tokens.gen.h"
#include "stack.h"
#include "tokenize.h"
#include "trie.h"
#include <string.h>

inline Tokenizer *Tokenizer_new(const char_t *src, Array *ident_array, const Allocator *allocator) {
  Tokenizer *const tokenizer = allocator->calloc(1, sizeof(Tokenizer));
  tokenizer->allocator = allocator;

  tokenizer->ident_trie = Trie_new(sizeof(char_t), get_char, allocator);
  tokenizer->context = MacroContext_new(allocator);
  tokenizer->framestack = Stack_new(allocator);
  MacroCallFrame_init(&tokenizer->frame);
  tokenizer->ident_array = ident_array;
  tokenizer->lineno = 1;
  tokenizer->column = 1;
  tokenizer->src = src;
  tokenizer->cost = 0;
  return tokenizer;
}

inline void Tokenizer_destroy(Tokenizer *const tokenizer) {
  MacroContext_destroy(tokenizer->context);
  Trie_destroy(tokenizer->ident_trie);
  Stack_clear(tokenizer->framestack);
  tokenizer->allocator->free(tokenizer->framestack);
  tokenizer->allocator->free(tokenizer);
}

uint32_t Tokenizer_frame_pos_to_array(Tokenizer *const tokenizer, Array /*<TokenPos>*/ *array) {
  Array_append(array, &tokenizer->frame.position, 1);

  MacroCallFrame frame = {};

  while (!Stack_empty(tokenizer->framestack)) {
    Stack_pop(tokenizer->framestack, &frame, sizeof(MacroCallFrame));
    Array_append(array, &frame.position, 1);
  }
  return Array_length(array);
}
