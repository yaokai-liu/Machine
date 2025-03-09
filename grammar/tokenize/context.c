/* License
 *
 * ${PROJ_DESCRIPTION}
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
#include "terminal.h"
#include "action-table.gen.h"


MacroContext *MacroContext_new(const Allocator *allocator) {
  MacroContext *context = allocator->calloc(1, sizeof(MacroContext));
  context->macroArray = Array_new(sizeof(Macro), enum_Macro, allocator);
  context->macroTrie = Trie_new(sizeof(char_t), getchar, allocator);
  context->call_stack = Stack_new(allocator);
  context->current_params = nullptr;
  context->current_args = nullptr;
  context->end_parse = false;
  context->in_parse = false;
  context->depth = 0;
  return context;
}

REFER(Macro) MacroContext_addMacro(MacroContext *context, Macro *macro) {
  Array_append(context->macroArray, macro, 1);
  REFER(Macro) v_macro = Array_last_virt(context->macroArray);
  Trie_set(context->macroTrie, macro->name->ptr, v_macro);
  return v_macro;
}


uint32_t MacroContext_getIdentParamIndex(MacroContext *context, Identifier *ident) {
  if (!context->current_params) { return 0; }
  const Identifier *idents = Array_first_real(context->current_params);
  const uint32_t count = Array_length(context->current_params);
  for (uint32_t i = 0; i < count; i ++) {
    if (Identifier_cmp(&idents[i], ident) == 0) {
      return i + 1;
    }
  }
  return 0;
}

inline void MacroContext_pushCallStack(MacroContext *context, MacroCall *call) {
  Stack_push(context -> call_stack, call, sizeof(MacroCall));
}

inline void MacroContext_popCallStack(MacroContext *context, MacroCall *call) {
  Stack_pop(context -> call_stack, call, sizeof(MacroCall));
}


void set_in_parse_true(MacroContext *context, void *) {
  context->in_parse = true;
}

void set_end_parse_true(MacroContext *context, void *) {
  context->end_parse = true;
}

void set_current_params(MacroContext *context, MacroParams *params) {
  params = (params == (void *) enum_MacroParams) ? nullptr : params;
  context->current_params = params;
}

void push_macro_call(MacroContext *context, MacroCall *call) {
  MacroContext_pushCallStack(context, call);
}

fn_ctx_act *macro_get_after_stack_action(uint32_t state) {
  switch (state) {
    case __IDENTIFIER_LEFT_PAREN_LEFT_BRACKET:
    case __MACRO_IDENTIFIER_LEFT_PAREN_MacroParams_RIGHT_PAREN_LEFT_BRACKET: {
      return set_in_parse_true;
    }
    case __IDENTIFIER_LEFT_PAREN_MacroArgs_RIGHT_PAREN:
    case __MACRO_IDENTIFIER_LEFT_PAREN_MacroParams_RIGHT_PAREN_LEFT_BRACKET_Tokens_RIGHT_BRACKET: {
      return set_end_parse_true;
    }
    default:{}
  }
  return nullptr;
}

fn_ctx_act *macro_get_after_reduce_action(uint32_t state) {
  switch (state) {
    case __MACRO_IDENTIFIER_LEFT_PAREN_MacroParams_RIGHT_PAREN: {
      return (fn_ctx_act *) set_current_params;
    }
    case __MacroCall: {
      return (fn_ctx_act *) push_macro_call;
    }
    default:{}
  }
  return nullptr;
}
