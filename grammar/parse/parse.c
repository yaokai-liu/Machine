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
#include "generated/machine/action-table.gen.h"
#include "generated/machine/reduce.gen.h"
#include "stack.h"
#include "target.h"

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, void *token, uint32_t type, const Allocator *allocator
);

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, void *args[], int32_t *states, uint32_t argc,
    const Allocator *allocator
);

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator);

#define MAX_ARGC 0x10
Machine *parse(Tokenizer *tokenizer, const char_t **err_msg, const Allocator *allocator) {
  void *result;
  int32_t state = 0;
  Terminal terminal = {};
  void *args[MAX_ARGC] = {};
  int32_t states[MAX_ARGC] = {};
  Stack *state_stack = Stack_new(allocator);
  Stack *token_stack = Stack_new(allocator);
  Stack_push(state_stack, &state, sizeof(int32_t));
  ParseContext *context = GContext_new(allocator);

  Tokenizer_next(tokenizer, &terminal);
  while (true) {
    const struct grammar_action *act = getParseAction(state, terminal.type);
    if (!act) {
      *err_msg = "unexpected token.";
      GContext_destroy(context);
      return clean_parse_stack(state_stack, token_stack, allocator);
    }
    if (act->action == stack) {
      state = act->offset;
      Stack_push(token_stack, &(terminal.value), sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_stack_actions(state);
      if (ctx_act) { ctx_act(context, terminal.value); }
      Tokenizer_next(tokenizer, &terminal);
    } else if (act->action == reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(void *));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, (int32_t *) &state, sizeof(int32_t));
      fn_reduce *reduce = MACHINE_PRODUCTS[act->offset];
      result = reduce(args, context, allocator);
      if (!result) {
        *err_msg = GContext_getErrorMessage(context);
        GContext_destroy(context);
        return failed_to_produce(state_stack, token_stack, args, states, act->count, allocator);
      }
      state = parseJumpState(state, act->type);
      if (state < 0) {
        GContext_destroy(context);
        *err_msg = "unexpected token.";
        return failed_to_get_next_state(state_stack, token_stack, result, act->type, allocator);
      }
      Stack_push(token_stack, &result, sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_reduce_actions(state);
      if (ctx_act) { ctx_act(context, terminal.value); }
      if (act->offset == __EXTEND_RULE__) { break; }
    } else {
      // never be touched
    }
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  Machine *machine = result;
  return machine;
}
