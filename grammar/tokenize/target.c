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

void MacroCallFrame_init(MacroCallFrame *frame) {
  frame->concatArray = nullptr;
  frame->macro = nullptr;
  frame->tokens = nullptr;
  frame->args = nullptr;
  frame->index = 0;
}

void releaseMacro(Macro *macro, const Allocator *) {
  if ((uint64_t) macro->params > Machine_TOKEN_MacroParams) { releasePrimeArray(macro->params); }
  releasePrimeArray(macro->tokens);
  if (macro->concatArray) {
    Array_reset(macro->concatArray, (destruct_t *) releaseConcat);
    Array_destroy(macro->concatArray);
  }
}

void releaseMacroArg(MacroArg *arg, const Allocator *allocator) {
  if (arg->type == Machine_TOKEN_Tokens) {
    releasePrimeArray(arg->target);
  } else {
    allocator->free(arg->target);
  }
  arg->target = nullptr;
}

void releaseMacroToken(Token *token, const Allocator *allocator) {
  switch (token->type) {
    case Machine_TOKEN_Macro: {
      releaseMacro(token->value, allocator);
      allocator->free(token->value);
      break;
    }
    case Machine_TOKEN_Concat:
    case Machine_TOKEN_Tokens:
    case Machine_TOKEN_MacroParams: {
      releasePrimeArray(token->value);
      break;
    }
    case Machine_TOKEN_MacroArgs: {
      Array_reset(token->value, (destruct_t *) releaseMacroArg);
      Array_destroy(token->value);
    }
    default: {
    }
  }
}

void releaseConcat(Concat *concat, const Allocator *) {
  Array_reset(concat, nullptr);
}
