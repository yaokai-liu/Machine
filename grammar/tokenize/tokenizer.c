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
#include "generated/macro/action-table.gen.h"
#include "generated/macro/reduce.gen.h"
#include "generated/tokens.gen.h"
#include "stack.h"
#include "tokenize.h"
#include "trie.h"

typedef struct Tokenizer {
  const Allocator *allocator;
  const char_t *src;
  uint32_t cost;
  uint32_t lineno;
  uint32_t column;
  MacroContext *context;

  Stack *call_stack;
  MacroCallFrame frame;
} Tokenizer;

uint32_t Tokenizer_parse(Tokenizer *tokenizer, Terminal *tp);
Macro *Tokenizer_hasMacro(Tokenizer *tokenizer, Identifier *ident);

Tokenizer *Tokenizer_new(const char_t *src, const Allocator *allocator) {
  Tokenizer *tokenizer = allocator->calloc(1, sizeof(Tokenizer));
  MacroContext *context = MacroContext_new(allocator);
  tokenizer->call_stack = Stack_new(allocator);
  tokenizer->allocator = allocator;
  tokenizer->context = context;
  tokenizer->frame.tokens = nullptr;
  tokenizer->frame.args = nullptr;
  tokenizer->frame.index = 0;
  tokenizer->src = src;
  tokenizer->cost = 0;
  return tokenizer;
}

#define pText (tokenizer->src + tokenizer->cost)
uint32_t Tokenizer_next(Tokenizer *tokenizer, Terminal * const terminal) {
  const uint32_t old_cost = tokenizer->cost;
  if (tokenizer->frame.tokens) {
    const Terminal *token = Array_real_addr(tokenizer->frame.tokens, tokenizer->frame.index++);
    if (token->type == enum_PLACE_HOLDER) {
      const uint32_t index = (uint32_t) (uint64_t) token->value;
      const MacroArg *arg = Array_real_addr(tokenizer->frame.args, index);
      if (arg->type == enum_IDENTIFIER) {
        terminal->type = arg->type;
        terminal->value = arg->target;
        return 0;
      } else {
        Stack_push(tokenizer->call_stack, &tokenizer->frame, sizeof(MacroCallFrame));
        tokenizer->frame.tokens = arg->target;
        tokenizer->frame.args = nullptr;
        tokenizer->frame.index = 0;
        return Tokenizer_next(tokenizer, terminal);
      }
    }
    tokenizer->allocator->memcpy(terminal, token, sizeof(Terminal));
    if (tokenizer->frame.index >= Array_length(tokenizer->frame.tokens)) {
      Stack_pop(tokenizer->call_stack, &tokenizer->frame, sizeof(MacroCallFrame));
    }
  } else {
    uint32_t cost = 0;
    tokenizer->cost += pass_space(pText, &tokenizer->lineno, &tokenizer->column);
    terminal->lineno = tokenizer->lineno;
    terminal->column = tokenizer->column;
    if ('\0' == *pText) {
      terminal->type = enum_TERMINATOR;
      terminal->value = nullptr;
      terminal->length = 0;
      return 0;
    }
    cost = single_tokenize(pText, terminal, tokenizer->allocator);
    if (0 == cost) { return 0; }
    tokenizer->cost += cost;
  }
  if (terminal->type == enum_MACRO) {
    Tokenizer_parse(tokenizer, terminal);
    return Tokenizer_next(tokenizer, terminal);
  }

  if (terminal->type == enum_IDENTIFIER) {
    REFER(Macro) v_macro = Tokenizer_hasMacro(tokenizer, (Identifier *) terminal->value);
    if (v_macro) {
      Tokenizer_parse(tokenizer, terminal);
      Stack_push(tokenizer->call_stack, &tokenizer->frame, sizeof(MacroCallFrame));
      MacroContext_makeFrame(tokenizer->context, &tokenizer->frame, v_macro);
      return Tokenizer_next(tokenizer, terminal);
    }
  }
  return tokenizer->cost - old_cost;
}
Macro *Tokenizer_hasMacro(Tokenizer *tokenizer, Identifier *ident) {
  const MacroContext * const context = tokenizer->context;
  return Trie_get(context->macroTrie, ident->ptr);
}
typedef void *fn_reduce(void *argv[], MacroContext *context, const Allocator *allocator);

extern fn_reduce * const MACRO_PRODUCTS[];

uint32_t Tokenizer_macro_next(Tokenizer *tokenizer, Terminal * const terminal) {
  if (tokenizer->context->end_parse) {
    terminal->type = enum_TERMINATOR;
    terminal->value = nullptr;
    return 0;
  }

  tokenizer->cost += pass_space(pText, &tokenizer->lineno, &tokenizer->column);
  terminal->lineno = tokenizer->lineno;
  terminal->column = tokenizer->column;
  if ('\0' == *pText) {
    terminal->type = enum_TERMINATOR;
    terminal->value = nullptr;
    terminal->length = 0;
    return 0;
  }

  const uint32_t cost = single_tokenize(pText, terminal, tokenizer->allocator);
  if (0 == cost) { return 0; }
  tokenizer->cost += cost;
  switch (terminal->type) {
    case enum_LEFT_BRACKET: {
      if (tokenizer->context->depth++ == 0) { return cost; }
      break;
    }
    case enum_RIGHT_BRACKET: {
      if (--tokenizer->context->depth == 0) { return cost; }
      break;
    }
    case enum_COMMA:
    case enum_LEFT_PAREN:
    case enum_RIGHT_PAREN:
    case enum_IDENTIFIER: {
      if (!tokenizer->context->in_parse) { return cost; }
      break;
    }
    default: {
    }
  }
  Terminal *token = tokenizer->allocator->calloc(1, sizeof(Terminal));
  tokenizer->allocator->memcpy(token, terminal, sizeof(Terminal));
  terminal->type = enum_TOKEN;
  terminal->value = token;
  return cost;
}

#define MAX_ARGC 0x10
uint32_t Tokenizer_parse(Tokenizer *tokenizer, Terminal * const tp) {
  void *result;
  int32_t state = 0;
  void *args[MAX_ARGC] = {};
  int32_t states[MAX_ARGC] = {};
  const Allocator * const allocator = tokenizer->allocator;
  Stack *state_stack = Stack_new(allocator);
  Stack *token_stack = Stack_new(allocator);
  Stack_push(state_stack, &state, sizeof(int32_t));
  MacroContext * const context = tokenizer->context;
  context->in_parse = false;
  context->end_parse = false;
  context->depth = 0;
  uint32_t cost = 0;

  while (true) {
    const struct grammar_action *act = getMacroParseAction(state, tp->type);
    if (!act) { return 0; }
    if (act->action == stack) {
      state = act->offset;
      Stack_push(token_stack, &(tp->value), sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = macro_get_after_stack_action(state);
      if (ctx_act) { ctx_act(context, tp->value); }
      cost += Tokenizer_macro_next(tokenizer, tp);
    } else if (act->action == reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(void *));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, (int32_t *) &state, sizeof(int32_t));
      fn_reduce *reduce = MACRO_PRODUCTS[act->offset];
      result = reduce(args, context, allocator);
      if (!result) { return 0; }
      state = macroParseJumpState(state, act->type);
      if (state < 0) { return 0; }
      fn_ctx_act *ctx_act = macro_get_after_reduce_action(state);
      if (ctx_act) { ctx_act(context, result); }
      Stack_push(token_stack, &result, sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      if (act->offset == __EXTEND_RULE__) { break; }
    } else {
      // never be touched
    }
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  return cost;
}
