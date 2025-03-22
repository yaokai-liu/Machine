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
 * Filename: reduce.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "generated/macro/reduce.gen.h"
#include "target.h"

MacroEntry *p_MacroEntry_0(Token argv[], MacroContext *, const Allocator *) {
  return (MacroEntry *) argv[0].value;
}
MacroEntry *p_MacroEntry_1(Token argv[], MacroContext *, const Allocator *) {
  return (MacroEntry *) argv[0].value;
}
MacroEntry *p__MacroEntry__(Token argv[], MacroContext *, const Allocator *) {
  return (MacroEntry *) argv[0].value;
}
Macro *p_Macro_0(Token argv[], MacroContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[1].value;
  MacroParams *params = (MacroParams *) argv[3].value;
  Tokens *tokens = (Tokens *) argv[6].value;

  if (MacroContext_findMacro(context, ident)) { return nullptr; }

  params = (params == (void *) enum_MacroParams) ? nullptr : params;

  Macro macro = {
      .name = ident, .params = params, .tokens = tokens, .concatArray = context->current_concatArray
  };

  return MacroContext_addMacro(context, &macro);
}

MacroArg *p_MacroArg_0(Token argv[], MacroContext *, const Allocator *allocator) {
  Tokens *tokens = (Tokens *) argv[1].value;

  MacroArg *arg = allocator->calloc(1, sizeof(MacroArg));
  arg->type = enum_Tokens;
  arg->target = tokens;

  return arg;
}
MacroArg *p_MacroArg_1(Token argv[], MacroContext *, const Allocator *allocator) {
  const Token *token = (Token *) &argv[0];

  MacroArg *arg = allocator->calloc(1, sizeof(MacroArg));
  Token *tp = allocator->calloc(1, sizeof(Token));
  arg->type = enum_IDENTIFIER;
  arg->target = tp;
  *tp = *token;

  return arg;
}

MacroArg *p_MacroArg_2(Token argv[], MacroContext *, const Allocator *allocator) {
  const Token *token = (Token *) &argv[0];

  MacroArg *arg = allocator->calloc(1, sizeof(MacroArg));
  Token *tp = allocator->calloc(1, sizeof(Token));
  arg->type = enum_NUMBER;
  arg->target = tp;
  *tp = *token;

  return arg;
}

MacroArgs *p_MacroArgs_0(Token argv[], MacroContext *, const Allocator *allocator) {
  MacroArgs *args = (MacroArgs *) argv[0].value;
  MacroArg *arg = (MacroArg *) argv[2].value;

  Array_append(args, arg, 1);
  allocator->free(arg);

  return args;
}
MacroArgs *p_MacroArgs_1(Token argv[], MacroContext *, const Allocator *allocator) {
  MacroArg *arg = (MacroArg *) argv[0].value;

  MacroArgs *args = Array_new(sizeof(MacroArg), enum_MacroArg, allocator);
  Array_append(args, arg, 1);
  allocator->free(arg);

  return args;
}

MacroArgs *p_MacroArgs_2(Token[], MacroContext *, const Allocator *) {
  return (MacroArgs *) (uint64_t) enum_MacroArgs;
}

MacroCall *p_MacroCall_0(Token argv[], MacroContext *context, const Allocator *) {
  Identifier *ident = (Identifier *) argv[0].value;
  MacroArgs *args = (MacroArgs *) argv[2].value;

  REFER(Macro) v_macro = AVLTree_get(context->macroTree, (uint64_t) ident);
  const Macro *macro = Array_virt2real(context->macroArray, v_macro);
  const MacroParams *params = macro->params;
  if (Array_length(args) != Array_length(params)) { return nullptr; }

  context->current_args = args;

  return (void *) enum_MacroCall;
}

MacroParams *p_MacroParams_0(Token argv[], MacroContext *, const Allocator *) {
  MacroParams *params = (MacroParams *) argv[0].value;
  Identifier *ident = (Identifier *) argv[2].value;

  Array_append(params, &ident, 1);

  return params;
}

MacroParams *p_MacroParams_1(Token argv[], MacroContext *context, const Allocator *allocator) {
  Identifier *ident = (Identifier *) argv[0].value;

  MacroParams *params = Array_new(sizeof(REFER(Identifier)), enum_IDENTIFIER, allocator);
  Array_append(params, &ident, 1);

  context->current_params = params;

  return params;
}

MacroParams *p_MacroParams_2(Token[], MacroContext *, const Allocator *) {
  return (MacroParams *) (uint64_t) enum_MacroParams;
}

#define identToPlaceHolder(ident)                                                \
  do {                                                                           \
    if ((ident)->type == enum_IDENTIFIER) {                                      \
      uint32_t index = MacroContext_getIdentParamIndex(context, (ident)->value); \
      if (index) {                                                               \
        (ident)->type = enum_PLACE_HOLDER;                                       \
        (ident)->value = (void *) (uint64_t) index - 1;                          \
      }                                                                          \
    }                                                                            \
  } while (false)

Tokens *p_Tokens_0(Token argv[], MacroContext *context, const Allocator *allocator) {
  Tokens *tokens = (Tokens *) argv[0].value;
  Token *token = (Token *) argv[1].value;

  identToPlaceHolder(token);

  Array_append(tokens, token, 1);
  allocator->free(token);

  return tokens;
}

Tokens *p_Tokens_1(Token argv[], MacroContext *context, const Allocator *) {
  Tokens *tokens = (Tokens *) argv[0].value;
  Concat *_concat = (Concat *) argv[1].value;

  Array_append(context->current_concatArray, _concat, 1);
  const Token *first = Array_first_real(_concat);
  const Token *last = Array_last_real(_concat);
  Token token = {
      .type = enum_Concat,
      .value = Array_last_virt(context->current_concatArray),
      .position = {first->position[0], last->position[1]}
  };

  Array_append(tokens, &token, 1);
  Array_destroy(_concat);

  return tokens;
}

Tokens *p_Tokens_2(Token argv[], MacroContext *context, const Allocator *allocator) {
  Token *token = (Token *) argv[0].value;

  identToPlaceHolder(token);

  Tokens *tokens = Array_new(sizeof(Token), enum_TOKEN, allocator);
  Array_append(tokens, token, 1);
  allocator->free(token);

  return tokens;
}

Tokens *p_Tokens_3(Token argv[], MacroContext *context, const Allocator *allocator) {
  Concat *_concat = (Concat *) argv[0].value;

  Array_append(context->current_concatArray, _concat, 1);
  const Token *first = Array_first_real(_concat);
  const Token *last = Array_last_real(_concat);
  Token token = {
      .type = enum_Concat,
      .value = Array_last_virt(context->current_concatArray),
      .position = {first->position[0], last->position[1]}
  };

  Tokens *tokens = Array_new(sizeof(Token), enum_TOKEN, allocator);
  Array_append(tokens, &token, 1);
  Array_destroy(_concat);

  return tokens;
}

Concat *p_Concat_0(Token argv[], MacroContext *context, const Allocator *allocator) {
  Token *left = argv[0].value;
  Token *right = argv[2].value;

  identToPlaceHolder(left);
  identToPlaceHolder(right);

  Concat *_concat = Array_new(sizeof(Token), enum_TOKEN, allocator);
  Array_append(_concat, left, 1);
  Array_append(_concat, right, 1);
  allocator->free(left);
  allocator->free(right);
  return _concat;
}

Concat *p_Concat_1(Token argv[], MacroContext *context, const Allocator *allocator) {
  Concat *_concat = argv[0].value;
  Token *token = argv[2].value;

  identToPlaceHolder(token);

  Array_append(_concat, token, 1);
  allocator->free(token);

  return _concat;
}
