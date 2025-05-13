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
 * Filename: parse.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-10
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "parse.h"
#include "action.h"
#include "enum.h"
#include "generated/macro/action-table.gen.h"
#include "generated/macro/rules.gen.h"
#include "tokenizer.h"

extern fn_macro_reduce * const MACRO_PRODUCTS[];

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
inline uint32_t Tokenizer_parse(Tokenizer *const tokenizer, Token * const token, ErrInfo * const err_info) {
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

  while (true) {
    const struct grammar_action *act = getMacroParseAction(state, token->type);
    if (!act) {
      err_info->pos[0] = token->position[0];
      err_info->pos[1] = token->position[1];
      err_info->code = ERROR_UNEXPECTED_TOKEN;
      err_info->stage = COMPILER_PARSE;
      err_info->token = token->type;
      tokenizer_clean_parse_stack(state_stack, token_stack, allocator);
      return ERROR_UNEXPECTED_TOKEN;
    }
    if (act->action == Macro_action_stack) {
      state = act->offset;
      err_info->state = state;
      Stack_push(token_stack, token, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_macro_ctx_act *ctx_act = macro_get_after_stack_action(state);
      if (ctx_act) { ctx_act(context, token->value); }
      uint32_t error = Tokenizer_macro_next(tokenizer, token, err_info);
      if (error != SUCCESS) {
        tokenizer_clean_parse_stack(state_stack, token_stack, allocator);
        return error;
      }
    } else if (act->action == Macro_action_reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(Token));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, &state, sizeof(int32_t));
      fn_macro_reduce *reduce = MACRO_PRODUCTS[act->offset];
      result.type = act->type;
      result.position[0].lineno = args[0].position[0].lineno;
      result.position[0].column = args[0].position[0].column;
      result.position[1].lineno = args[act->count - 1].position[1].lineno;
      result.position[1].column = args[act->count - 1].position[1].column;
      result.value = reduce(args, context, err_info, allocator);
      if (!result.value) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        tokenizer_failed_to_produce(state_stack, token_stack, args, act->count, allocator);
        return ERROR_FAILED_TO_PRODUCE;
      }
      state = macroParseJumpState(state, act->type);
      err_info->state = state;
      if (state < 0) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->code = ERROR_UNEXPECTED_TOKEN;
        err_info->stage = COMPILER_PARSE;
        err_info->token = result.type;
        tokenizer_failed_to_get_next_state(state_stack, token_stack, &result, allocator);
        return ERROR_FAILED_TO_GET_NEXT_STATE;
      }
      fn_macro_ctx_act *ctx_act = macro_get_after_reduce_action(state);
      if (ctx_act) { ctx_act(context, result.value); }
      if (act->offset == enum_Macro_MacroEntry_EXT) { break; }
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
  return SUCCESS;
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
