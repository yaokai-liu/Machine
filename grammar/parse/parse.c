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
#include "err.h"
#include "generated/machine/action-table.gen.h"
#include "generated/machine/reduce.gen.h"
#include "stack.h"
#include "target.h"

#define MAX_ARGC 0x10
Machine *parse(Tokenizer *tokenizer, ErrInfo *err_info, const Allocator *allocator) {
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
      err_info->pos[0] = token.position[0];
      err_info->pos[1] = token.position[1];
      err_info->msg = "unexpected token.";
      GContext_destroy(context);
      return clean_parse_stack(state_stack, token_stack, allocator);
    }
    if (act->action == stack) {
      state = act->offset;
      Stack_push(token_stack, &token, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_stack_actions(state);
      if (ctx_act) { ctx_act(context, token.value); }
      Tokenizer_next(tokenizer, &token, err_info);
    } else if (act->action == reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(Token));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, (int32_t *) &state, sizeof(int32_t));
      fn_reduce *reduce = MACHINE_PRODUCTS[act->offset];
      result.type = act->type;
      result.position[0].lineno = args[0].position[0].lineno;
      result.position[0].column = args[0].position[0].column;
      result.position[1].lineno = args[act->count - 1].position[1].lineno;
      result.position[1].column = args[act->count - 1].position[1].column;
      result.value = reduce(args, context, allocator);
      if (!result.value) {
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->msg = GContext_getErrorMessage(context);
        GContext_destroy(context);
        return failed_to_produce(state_stack, token_stack, args, act->count, allocator);
      }
      state = parseJumpState(state, act->type);
      if (state < 0) {
        GContext_destroy(context);
        err_info->pos[0] = result.position[0];
        err_info->pos[1] = result.position[1];
        err_info->msg = "unexpected token.";
        return failed_to_get_next_state(state_stack, token_stack, &result, allocator);
      }
      Stack_push(token_stack, &result, sizeof(Token));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_reduce_actions(state);
      if (ctx_act) { ctx_act(context, token.value); }
      if (act->offset == __EXTEND_RULE__) { break; }
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
