/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: parse.c
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "parse.h"
#include "action-table.h"
#include "context.h"
#include "reduce.gen.h"
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
Machine *
    parse(const Terminal *tokens, uint32_t *cost, void *getCodegen, const Allocator *allocator) {
  void *result;
  int32_t state = 0;
  const Terminal *tp = tokens;
  void *args[MAX_ARGC] = {};
  int32_t states[MAX_ARGC] = {};
  Stack *state_stack = Stack_new(allocator);
  Stack *token_stack = Stack_new(allocator);
  Stack_push(state_stack, &state, sizeof(int32_t));
  GContext *context = GContext_new(allocator);
  GContext_setCodegen(context, getCodegen);

  while (true) {
    const struct grammar_action *act = getAction(state, tp->type);
    if (!act) {
      *cost = (uint32_t) (uint64_t) (tp - tokens);
      GContext_destroy(context);
      return clean_parse_stack(state_stack, token_stack, allocator);
    }
    if (act->action == stack) {
      state = act->offset;
      Stack_push(token_stack, &(tp->value), sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_stack_actions(state);
      if (ctx_act) { ctx_act(context, tp->value); }
      tp++;
    } else if (act->action == reduce) {
      Stack_pop(token_stack, args, act->count * sizeof(void *));
      Stack_pop(state_stack, states, act->count * sizeof(int32_t));
      Stack_top(state_stack, (int32_t *) &state, sizeof(int32_t));
      fn_reduce *reduce = PRODUCTS[act->offset];
      result = reduce(args, context, allocator);
      if (!result) {
        *cost = (uint32_t) (uint64_t) (tp - tokens);
        GContext_destroy(context);
        return failed_to_produce(state_stack, token_stack, args, states, act->count, allocator);
      }
      state = jump(state, act->type);
      if (state < 0) {
        *cost = (uint32_t) (uint64_t) (tp - tokens);
        GContext_destroy(context);
        return failed_to_get_next_state(state_stack, token_stack, result, act->type, allocator);
      }
      Stack_push(token_stack, &result, sizeof(void *));
      Stack_push(state_stack, &state, sizeof(int32_t));
      fn_ctx_act *ctx_act = get_after_reduce_actions(state);
      if (ctx_act) { ctx_act(context, tp->value); }
      if (act->offset == __EXTEND_RULE__) { break; }
    } else {
      // never be touched
    }
  }
  Stack_clear(token_stack);
  Stack_clear(state_stack);
  allocator->free(token_stack);
  allocator->free(state_stack);
  *cost = (uint32_t) (uint64_t) (tp - tokens);
  Machine *machine = result;
  machine->context = context;
  return machine;
}
