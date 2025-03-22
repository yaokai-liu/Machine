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
#include <string.h>

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

uint32_t Tokenizer_parse(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_single_tokenize(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_next_in_src(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_macro_next(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
void Tokenizer_next_in_tokens(Tokenizer *tokenizer, Token *token);
void Tokenizer_enter_macro(Tokenizer *tokenizer, Token *token, REFER(Macro) v_macro);
void Tokenizer_try_exit_macro(Tokenizer *tokenizer);
void Tokenizer_concat_to_token(Tokenizer *tokenizer, const Concat *concat, Token *token);

inline Tokenizer *Tokenizer_new(const char_t *src, Array *ident_array, const Allocator *allocator) {
  Tokenizer *tokenizer = allocator->calloc(1, sizeof(Tokenizer));
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

inline void Tokenizer_destroy(Tokenizer *tokenizer) {
  MacroContext_destroy(tokenizer->context);
  Trie_destroy(tokenizer->ident_trie);
  Stack_clear(tokenizer->framestack);
  tokenizer->allocator->free(tokenizer->framestack);
  tokenizer->allocator->free(tokenizer);
}

#define pText (tokenizer->src + tokenizer->cost)
inline uint32_t
    Tokenizer_single_tokenize(Tokenizer *tokenizer, Token * const token, ErrInfo *err_info) {
  const uint32_t old_cost = tokenizer->cost;
  uint32_t cost = pass_space(pText, &tokenizer->lineno, &tokenizer->column);
  tokenizer->cost += cost;

  Terminal terminal = {};
  terminal.lineno = tokenizer->lineno;
  terminal.column = tokenizer->column;
  cost = single_tokenize(pText, &terminal, tokenizer->allocator);
  if (0 == cost) {
    err_info->msg = "un recognized symbol.";
    err_info->pos[0].lineno = tokenizer->lineno;
    err_info->pos[1].column = tokenizer->column;
    err_info->pos[1] = err_info->pos[0];
    token->type = enum_TERMINATOR;
    token->value = nullptr;
    return 0;
  }
  if (terminal.type == enum_IDENTIFIER) {
    REFER(Identifier) ident = Trie_get(tokenizer->ident_trie, terminal.value);
    if (!ident) {
      Array_append(tokenizer->ident_array, terminal.value, terminal.length + 1);
      ident = Array_last_virt(tokenizer->ident_array) - terminal.length;
      Trie_set(tokenizer->ident_trie, terminal.value, ident);
    }
    tokenizer->allocator->free(terminal.value);
    terminal.value = ident;
  }

  terminal_to_token(token, &terminal);

  tokenizer->cost += cost;
  tokenizer->column += cost;

  return tokenizer->cost - old_cost;
}

inline uint32_t
    Tokenizer_next_in_src(Tokenizer *tokenizer, Token * const token, ErrInfo *err_info) {
  uint32_t cost = Tokenizer_single_tokenize(tokenizer, token, err_info);
  while (token->type == enum_MACRO) {
    cost += Tokenizer_parse(tokenizer, token, err_info);
    cost += Tokenizer_single_tokenize(tokenizer, token, err_info);
  }
  return cost;
}

inline void Tokenizer_next_in_tokens(Tokenizer *tokenizer, Token * const token) {
  Token *tp = Array_real_addr(tokenizer->frame.tokens, tokenizer->frame.index++);
  TokenPos position = {tp->position[0], tp->position[1]};
  if (tp->type == enum_Concat) {
    const Concat *concat = Array_virt2real(tokenizer->frame.concatArray, tp->value);
    Tokenizer_concat_to_token(tokenizer, concat, token);
    token->position[0] = position[0];
    token->position[1] = position[1];
    return;
  }
  while (tp->type == enum_PLACE_HOLDER) {
    const uint32_t index = (uint32_t) (uint64_t) tp->value;
    MacroArg *arg = Array_real_addr(tokenizer->frame.args, index);
    if (arg->type != enum_Tokens) {
      tp = arg->target;
      break;
    }
    Stack_push(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
    tokenizer->frame.tokens = arg->target;
    tokenizer->frame.args = nullptr;
    tokenizer->frame.index = 0;
    tp = Array_real_addr(tokenizer->frame.tokens, tokenizer->frame.index++);
  }
  token->type = tp->type;
  token->value = tp->value;
  token->position[0] = position[0];
  token->position[1] = position[1];
}
void Tokenizer_concat_to_token(Tokenizer *tokenizer, const Concat *concat, Token * const token) {
  const Token *tokens = Array_first_real(concat);
  const uint32_t count = Array_length(concat);

  // push strings to ident_array
  Array *ident_array = Array_new(sizeof(char_t), enum_IDENTIFIER, tokenizer->allocator);
  for (uint32_t i = 0; i < count; i++) {
    const Token *tp = &tokens[i];
    if (tokens[i].type == enum_PLACE_HOLDER) {
      const uint32_t index = (uint32_t) (uint64_t) tokens[i].value;
      MacroArg *arg = Array_real_addr(tokenizer->frame.args, index);
      tp = arg->target;
    }
    if (tp->type != enum_IDENTIFIER) {
      releasePrimeArray(ident_array);
      token->type = enum_BAD_TOKEN;
      token->value = nullptr;
      return;
    }
    const char_t *sym_str = Array_virt2real(tokenizer->ident_array, tp->value);
    const uint32_t sym_len = strlen(sym_str);
    Array_append(ident_array, sym_str, sym_len);
  }
  Array_append(ident_array, "\0", 1);

  const char_t *sym_str = Array_first_real(ident_array);
  REFER(Identifier) v_sym = Trie_get(tokenizer->ident_trie, sym_str);
  if (!v_sym) {
    // add an identifier record
    v_sym = ((char_t *) Array_last_virt(tokenizer->ident_array)) + 1;
    Array_concat(tokenizer->ident_array, ident_array);
    Trie_set(tokenizer->ident_trie, sym_str, v_sym);
  }

  releasePrimeArray(ident_array);
  token->type = enum_IDENTIFIER;
  token->value = v_sym;
}

inline void Tokenizer_enter_macro(Tokenizer *tokenizer, Token * const token, REFER(Macro) v_macro) {
  Stack_push(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
  MacroContext_makeFrame(tokenizer->context, &tokenizer->frame, v_macro);
  tokenizer->frame.position[0] = token->position[0];
  tokenizer->frame.position[1] = token->position[1];
  Tokenizer_next_in_tokens(tokenizer, token);
}
inline void Tokenizer_try_exit_macro(Tokenizer *tokenizer) {
  while (tokenizer->frame.tokens && tokenizer->frame.index >= Array_length(tokenizer->frame.tokens)
  ) {
    if (tokenizer->frame.args) {
      Array_reset(tokenizer->frame.args, (destruct_t *) releaseMacroArg);
      Array_destroy(tokenizer->frame.args);
    }
    Stack_pop(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
  }
}

inline uint32_t Tokenizer_next(Tokenizer *tokenizer, Token * const token, ErrInfo *err_info) {
  uint32_t cost = 0;
  Tokenizer_try_exit_macro(tokenizer);
  if (tokenizer->frame.tokens) {
    Tokenizer_next_in_tokens(tokenizer, token);
  } else {
    cost += Tokenizer_next_in_src(tokenizer, token, err_info);
  }
  if (token->type != enum_IDENTIFIER) { return cost; }
  REFER(Macro) v_macro = MacroContext_findMacro(tokenizer->context, token->value);
  if (!v_macro) { return cost; }
  cost += Tokenizer_parse(tokenizer, token, err_info);
  Tokenizer_enter_macro(tokenizer, token, v_macro);
  return cost;
}

typedef void *fn_reduce(Token argv[], MacroContext *context, const Allocator *allocator);

extern fn_reduce * const MACRO_PRODUCTS[];

inline uint32_t Tokenizer_macro_next(Tokenizer *tokenizer, Token * const token, ErrInfo *err_info) {
  if (tokenizer->context->end_parse) {
    token->position[0].lineno = tokenizer->lineno;
    token->position[0].column = tokenizer->column;
    token->position[1].lineno = tokenizer->lineno;
    token->position[1].column = tokenizer->column;
    token->type = enum_TERMINATOR;
    token->value = nullptr;
    return 0;
  }
  const uint32_t cost = tokenizer->frame.tokens ? Tokenizer_next_in_tokens(tokenizer, token),
                 0 : Tokenizer_single_tokenize(tokenizer, token, err_info);
  switch (token->type) {
    case enum_MACRO:
    case enum_CONCAT: {
      return cost;
    }
    case enum_LEFT_BRACKET: {
      if (tokenizer->context->depth++ == 0) { return cost; }
      break;
    }
    case enum_RIGHT_BRACKET: {
      if (--tokenizer->context->depth == 0) { return cost; }
      break;
    }
    case enum_COMMA:
    case enum_NUMBER:
    case enum_LEFT_PAREN:
    case enum_RIGHT_PAREN:
    case enum_IDENTIFIER: {
      if (!tokenizer->context->in_macro) { return cost; }
      break;
    }
    default: {
    }
  }

  Token *tp = tokenizer->allocator->calloc(1, sizeof(Token));
  *tp = *token;

  token->type = enum_TOKEN;
  token->value = tp;
  return cost;
}

void tokenizer_failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, Token *result, const Allocator *allocator
);

void tokenizer_failed_to_produce(
    Stack *state_stack, Stack *token_stack, Token args[], uint32_t argc, const Allocator *allocator
);

void tokenizer_clean_parse_stack(
    Stack *state_stack, Stack *token_stack, const Allocator *allocator
);

#define MAX_ARGC 0x10
inline uint32_t Tokenizer_parse(Tokenizer *tokenizer, Token * const token, ErrInfo *err_info) {
  int32_t state = 0;
  Token result = {};
  Token args[MAX_ARGC] = {};
  int32_t states[MAX_ARGC] = {};
  const Allocator * const allocator = tokenizer->allocator;
  Stack *state_stack = Stack_new(allocator);
  Stack *token_stack = Stack_new(allocator);
  Stack_push(state_stack, &state, sizeof(int32_t));
  MacroContext * const context = tokenizer->context;
  context->in_macro = false;
  context->end_parse = false;
  context->depth = 0;
  uint32_t cost = 0;

  while (true) {
    const struct grammar_action *act = getMacroParseAction(state, token->type);
    if (!act) {
      err_info->pos[0] = token->position[0];
      err_info->pos[1] = token->position[1];
      err_info->msg = "unexpected token when parse macro.";
      tokenizer_clean_parse_stack(state_stack, token_stack, allocator);
      return 0;
    }
    if (act->action == stack) {
      state = act->offset;
      Stack_push(token_stack, token, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = macro_get_after_stack_action(state);
      if (ctx_act) { ctx_act(context, token->value); }
      cost += Tokenizer_macro_next(tokenizer, token, err_info);
    } else if (act->action == reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(Token));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, &state, sizeof(int32_t));
      fn_reduce *reduce = MACRO_PRODUCTS[act->offset];
      result.type = act->type;
      result.position[0].lineno = args[0].position[0].lineno;
      result.position[0].column = args[0].position[0].column;
      result.position[1].lineno = args[act->count - 1].position[1].lineno;
      result.position[1].column = args[act->count - 1].position[1].column;
      result.value = reduce(args, context, allocator);
      if (!result.value) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->msg = "failed to product.";
        tokenizer_failed_to_produce(state_stack, token_stack, args, act->count, allocator);
        return 0;
      }
      state = macroParseJumpState(state, act->type);
      if (state < 0) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->msg = "failed to goto next state.";
        tokenizer_failed_to_get_next_state(state_stack, token_stack, &result, allocator);
        return 0;
      }
      fn_ctx_act *ctx_act = macro_get_after_reduce_action(state);
      if (ctx_act) { ctx_act(context, result.value); }
      if (act->offset == __EXTEND_RULE__) { break; }
      Stack_push(token_stack, &result, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
    } else {
      // never be touched
    }
  }
  *token = result;
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  return cost;
}

void tokenizer_failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, Token *result, const Allocator *allocator
) {
  int32_t state = 0;
  Stack_top(state_stack, (&state), sizeof(int32_t));
  releaseMacroToken(result, allocator);
  allocator->free(result);
  return tokenizer_clean_parse_stack(state_stack, token_stack, allocator);
}

void tokenizer_failed_to_produce(
    Stack *state_stack, Stack *token_stack, Token args[], uint32_t argc, const Allocator *allocator
) {
  for (uint32_t i = 0; i < argc; i++) { releaseMacroToken(&args[i], allocator); }
  return tokenizer_clean_parse_stack(state_stack, token_stack, allocator);
}

void tokenizer_clean_parse_stack(
    Stack *state_stack, Stack *token_stack, const Allocator *allocator
) {
  Token token = {};
  while (!Stack_empty(token_stack)) {
    Stack_pop(token_stack, &token, sizeof(Token));
    releaseMacroToken(&token, allocator);
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
}

uint32_t Tokenizer_frame_pos_to_array(Tokenizer *tokenizer, Array /*<TokenPos>*/ *array) {
  Array_append(array, &tokenizer->frame.position, 1);

  MacroCallFrame frame = {};

  while (!Stack_empty(tokenizer->framestack)) {
    Stack_pop(tokenizer->framestack, &frame, sizeof(MacroCallFrame));
    Array_append(array, &frame.position, 1);
  }
  return Array_length(array);
}
