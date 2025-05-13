/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
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
 * Project Name: machine
 * Module Name: grammar/parse
 * Filename: parse.c
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "parse.h"
#include "action.h"
#include "context.h"
#include "enum.h"
#include "err.h"
#include "generated/machine/action-table.gen.h"
#include "generated/machine/rules.gen.h"
#include "stack.h"
#include "target.h"

extern fn_parse_reduce * const MACHINE_PRODUCTS[];

#define MAX_ARGC 0x10
Machine *parse(Tokenizer *const tokenizer, ErrInfo * const err_info, const Allocator *allocator) {
  int32_t state = 0;
  Token token = {}, result = {};
  Token args[MAX_ARGC] = {};
  int32_t states[MAX_ARGC] = {};
  Stack *state_stack = Stack_new(allocator);
  Stack *token_stack = Stack_new(allocator);
  Stack_push(state_stack, &state, sizeof(int32_t));
  ParseContext *context = GContext_new(allocator);

  Tokenizer_next(tokenizer, &token, err_info);
  while (true) {
    const struct grammar_action *act = getParseAction(state, token.type);
    if (!act) {
      if (token.type == enum_SEMICOLON) {
        uint32_t error = Tokenizer_next(tokenizer, &token, err_info);
        if (error != SUCCESS) { return clean_parse_stack(state_stack, token_stack, allocator); }
        continue;
      } else {
        err_info->pos[0] = token.position[0];
        err_info->pos[1] = token.position[1];
        err_info->code = ERROR_UNEXPECTED_TOKEN;
        err_info->stage = COMPILER_PARSE;
        err_info->token = token.type;
        GContext_destroy(context);
        return clean_parse_stack(state_stack, token_stack, allocator);
      }
    }
    if (act->action == Parse_action_stack) {
      state = act->offset;
      err_info->state = state;
      Stack_push(token_stack, &token, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_parse_ctx_act *ctx_act = get_after_stack_actions(state);
      if (ctx_act) { ctx_act(context, token.value); }
      uint32_t error = Tokenizer_next(tokenizer, &token, err_info);
      if (error != SUCCESS) { return clean_parse_stack(state_stack, token_stack, allocator); }
    } else if (act->action == Parse_action_reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(Token));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, (int32_t *) &state, sizeof(int32_t));
      fn_parse_reduce *reduce = MACHINE_PRODUCTS[act->offset];
      result.type = act->type;
      result.position[0].lineno = args[0].position[0].lineno;
      result.position[0].column = args[0].position[0].column;
      result.position[1].lineno = args[act->count - 1].position[1].lineno;
      result.position[1].column = args[act->count - 1].position[1].column;
      result.value = reduce(args, context, err_info, allocator);
      if (!result.value) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        // assume other error infos filled.
        GContext_destroy(context);
        return failed_to_produce(state_stack, token_stack, args, act->count, allocator);
      }
      state = parseJumpState(state, act->type);
      err_info->state = state;
      if (state < 0) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->code = ERROR_UNEXPECTED_TOKEN;
        err_info->stage = COMPILER_PARSE;
        err_info->token = result.type;
        GContext_destroy(context);
        return failed_to_get_next_state(state_stack, token_stack, &result, allocator);
      }
      Stack_push(token_stack, &result, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_parse_ctx_act *ctx_act = get_after_reduce_actions(state);
      if (ctx_act) { ctx_act(context, token.value); }
      if (act->offset == enum_Parse_Machine_EXT) { break; }
    } else {
      // never be touched
    }
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  Machine *machine = result.value;
  return machine;
}
