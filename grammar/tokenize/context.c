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
 * Filename: context.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "context.h"
#include "generated/macro/action-table.gen.h"
#include "terminal.h"

MacroContext *MacroContext_new(const Allocator *allocator) {
  MacroContext *context = allocator->calloc(1, sizeof(MacroContext));
  context->allocator = allocator;
  context->macroArray = Array_new(sizeof(Macro), Machine_TOKEN_Macro, allocator);
  context->macroTree = AVLTree_new(allocator, nullptr);
  context->current_concatArray = nullptr;
  context->current_params = nullptr;
  context->current_args = nullptr;
  context->end_parse = false;
  context->in_macro = false;
  context->depth = 0;
  return context;
}

void MacroContext_destroy(MacroContext *context) {
  Array_reset(context->macroArray, (destruct_t *) releaseMacro);
  Array_destroy(context->macroArray);
  AVLTree_destroy(context->macroTree, nullptr);
  if (context->current_params) { releasePrimeArray(context->current_params); }
  if (context->current_args) {
    Array_reset(context->current_args, (destruct_t *) releaseMacroArg);
    Array_destroy(context->current_args);
  }
  if (context->current_concatArray) {
    Array_reset(context->current_concatArray, (destruct_t *) releaseConcat);
    Array_destroy(context->current_concatArray);
  }
  context->allocator->free(context);
}

inline REFER(Macro) MacroContext_addMacro(MacroContext *context, Macro *macro) {
  Array_append(context->macroArray, macro, 1);
  REFER(Macro) v_macro = Array_last_virt(context->macroArray);
  AVLTree_set(context->macroTree, (uint64_t) macro->name, v_macro);
  return v_macro;
}

inline REFER(Macro) MacroContext_findMacro(MacroContext *context, REFER(Identifier) ident) {
  return AVLTree_get(context->macroTree, (uint64_t) ident);
}

inline const Macro *MacroContext_macroReal(MacroContext *context, REFER(Macro) v_macro) {
  return Array_virt2real(context->macroArray, v_macro);
}

inline uint32_t
    MacroContext_getIdentParamIndex(MacroContext *context, const REFER(Identifier) ident) {
  if (!context->current_params) { return 0; }
  REFER(Identifier) * const idents = Array_first_real(context->current_params);
  const uint32_t count = Array_length(context->current_params);
  for (uint32_t i = 0; i < count; i++) {
    if (idents[i] == ident) { return i + 1; }
  }
  return 0;
}

inline MacroCallFrame *
    MacroContext_makeFrame(MacroContext *context, MacroCallFrame *frame, REFER(Macro) v_macro) {
  const Macro *macro = Array_virt2real(context->macroArray, v_macro);
  frame->concatArray = macro->concatArray;
  frame->args = context->current_args;
  frame->macro = v_macro;
  frame->tokens = macro->tokens;
  frame->index = 0;

  context->current_args = nullptr;

  return frame;
}

void set_in_parse_true(MacroContext *context, void *) {
  context->in_macro = true;
}

void set_in_parse_false(MacroContext *context, void *) {
  context->in_macro = false;
}

void set_end_parse_true(MacroContext *context, void *) {
  context->end_parse = true;
}

void set_in_parse_false_end_parse_true(MacroContext *context, void *) {
  context->in_macro = false;
  context->end_parse = true;
}

void create_current_concat_array(MacroContext *context, void *) {
  context->current_concatArray = Array_new(sizeof_array, Machine_TOKEN_Concat, context->allocator);
  set_in_parse_true(context, nullptr);
}

void clear_current_arrays(MacroContext *context, void *) {
  context->current_concatArray = nullptr;
  context->current_params = nullptr;
  // not set context->current_args to nullptr, because MacroCall is executed directly.
}

fn_macro_ctx_act *macro_get_after_stack_action(uint32_t state) {
  switch (state) {
    case Macro_state_MACRO_IDENTIFIER_LEFT_BRACKET:
    case Macro_state_IDENTIFIER_LEFT_PAREN_LEFT_BRACKET: {
      return set_in_parse_true;
    }
    case Macro_state_MACRO_IDENTIFIER_LEFT_PAREN_MacroParams_RIGHT_PAREN_LEFT_BRACKET: {
      return create_current_concat_array;
    }
    case Macro_state_IDENTIFIER_LEFT_PAREN_LEFT_BRACKET_Tokens_RIGHT_BRACKET: {
      return set_in_parse_false;
    }
    case Macro_state_MACRO_IDENTIFIER_LEFT_BRACKET_Tokens_RIGHT_BRACKET: {
      return set_in_parse_false_end_parse_true;
    }
    case Macro_state_IDENTIFIER_LEFT_PAREN_MacroArgs_RIGHT_PAREN:
    case Macro_state_MACRO_IDENTIFIER_LEFT_PAREN_MacroParams_RIGHT_PAREN_LEFT_BRACKET_Tokens_RIGHT_BRACKET: {
      return set_end_parse_true;
    }
    default: {
    }
  }
  return nullptr;
}

fn_macro_ctx_act *macro_get_after_reduce_action(uint32_t state) {
  switch (state) {
    case Macro_state_MacroEntry: {
      return clear_current_arrays;
    }
    default: {
    }
  }
  return nullptr;
}
