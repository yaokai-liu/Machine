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
 * Filename: target.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-10
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include <string.h>

void concat_to_token(Concat *concat, Token *token, Array *ident_array) {
  const char_t *left_str = nullptr, *right_str = nullptr;

  if (concat->left.type == enum_IDENTIFIER) {
    left_str = Array_virt2real(ident_array, concat->left.value);
  }
  const uint32_t left_len = strlen(left_str);
  if (concat->right.type == enum_IDENTIFIER) {
    right_str = Array_virt2real(ident_array, concat->right.value);
  }
  const uint32_t right_len = strlen(left_str);

  uint32_t str_0 = Array_length(ident_array);
  Array_append(ident_array, left_str, left_len);
  Array_append(ident_array, right_str, right_len);
  Array_append(ident_array, "\0", 1);

  token->type = enum_IDENTIFIER;
  token->value = Array_virt_addr(ident_array, str_0);
}

void releaseMacro(Macro *macro, const Allocator *) {
  releasePrimeArray(macro->params);
  releasePrimeArray(macro->tokens);
}

void releaseMacroArg(MacroArg *arg, const Allocator *allocator) {
  allocator->free(arg->target);
  arg->target = nullptr;
}

void releaseMacroToken(Token *token, const Allocator *allocator) {
  switch (token->type) {
    case enum_MACRO: {
      releaseMacro(token->value, allocator);
      allocator->free(token->value);
      break;
    }
    case enum_Tokens:
    case enum_MacroParams: {
      releasePrimeArray(token->value);
      break;
    }
    case enum_MacroArgs: {
      Array_reset(token->value, (destruct_t *) releaseMacroArg);
      Array_destroy(token->value);
    }
    default: {
    }
  }
}
