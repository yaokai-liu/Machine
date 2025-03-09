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
 * Filename: reduce.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include "reduce.gen.h"

Entry * p_Entry_0(void *[], MacroContext *, const Allocator *) {
  return (Entry *) (uint64_t) (enum_Entry);
}
Entry * p_Entry_1(void *[], MacroContext *, const Allocator *) {
  return (Entry *) (uint64_t) (enum_Entry);
}
Entry * p__Entry__(void * argv[], MacroContext *, const Allocator *) {
  return argv[0];
}
Macro * p_Macro_0(void * argv[], MacroContext *context, const Allocator *) {
    Identifier *ident = (Identifier *) argv[1];
    MacroParams *params = (MacroParams *) argv[3];
    Tokens *tokens = (Tokens *) argv[6];

    params = (params == (void *) enum_MacroParams) ? nullptr : params;

    Macro macro = { .name = ident, .params = params, .tokens = tokens };
    return MacroContext_addMacro(context, &macro);
}

MacroArg * p_MacroArg_0(void * argv[], MacroContext *, const Allocator * allocator) {
  Tokens *tokens = (Tokens *) argv[1];

  MacroArg *arg = allocator->calloc(1, sizeof(MacroArg));
  arg->type = enum_Tokens;
  arg->target = tokens;

  return arg;
}
MacroArg * p_MacroArg_1(void * argv[], MacroContext *, const Allocator * allocator) {
  Identifier *ident = (Identifier *) argv[0];

  MacroArg *arg = allocator->calloc(1, sizeof(MacroArg));
  arg->type = enum_IDENTIFIER;
  arg->target = ident;

  return arg;
}
MacroArgs * p_MacroArgs_0(void * argv[], MacroContext *, const Allocator * allocator) {
  MacroArgs *args = (MacroArgs *) argv[0];
  MacroArg *arg = (MacroArg *) argv[2];

  Array_append(args, arg, 1);
  allocator->free(arg);

  return args;
}
MacroArgs * p_MacroArgs_1(void * argv[], MacroContext *, const Allocator * allocator) {
  MacroArg *arg = (MacroArg *) argv[0];


  MacroArgs *args = Array_new(sizeof(MacroArg), enum_MacroArg, allocator);
  Array_append(args, arg, 1);
  allocator->free(arg);

  return args;
}

MacroArgs * p_MacroArgs_2(void *[], MacroContext *, const Allocator *) {
  return (MacroArgs *) (uint64_t) enum_MacroArgs;
}

MacroCall * p_MacroCall_0(void * argv[], MacroContext *context, const Allocator * allocator) {
  Identifier *ident = (Identifier *) argv[0];
  MacroArgs *args = (MacroArgs *) argv[2];

  REFER(Macro) v_macro = Trie_get(context->macroTrie, ident->ptr);
  const Macro *macro = Array_vert2real(context->macroArray, v_macro);
  const MacroParams *params = macro->params;
  if (Array_length(args) != Array_length(params)) { return nullptr; }

  MacroCall *macro_call = allocator->calloc(1, sizeof(MacroCall));
  macro_call->name = ident;
  macro_call->args = args;

  return macro_call;
}

MacroParams * p_MacroParams_0(void * argv[], MacroContext *, const Allocator * allocator) {
  MacroParams *params = (MacroParams *) argv[0];
  Identifier *ident = (Identifier *) argv[2];

  Array_append(params, ident, 1);
  allocator->free(ident);

  return params;
}

MacroParams * p_MacroParams_1(void * argv[], MacroContext *, const Allocator * allocator) {
  Identifier *ident = (Identifier *) argv[0];


  MacroParams *params = Array_new(sizeof(Identifier), enum_IDENTIFIER, allocator);
  Array_append(params, ident, 1);
  allocator->free(ident);

  return params;
}

MacroParams * p_MacroParams_2(void *[], MacroContext *, const Allocator *) {
  return (MacroParams *) (uint64_t) enum_MacroParams;
}

Tokens * p_Tokens_0(void *argv[], MacroContext *context, const Allocator *allocator) {
  Tokens *tokens = (Tokens *) argv[0];
  Terminal *token = (Terminal *) argv[1];

  if (token->type == enum_IDENTIFIER) {
    uint32_t index = MacroContext_getIdentParamIndex(context, token->value);
    if (index) {
      releaseIdentifier(token->value, allocator);
      allocator->free(token->value);
      token->type = enum_PLACE_HOLDER;
      token->value = (void *) (uint64_t) index - 1;
    }
  }

  Array_append(tokens, token, 1);
  return tokens;
}

Tokens * p_Tokens_1(void * argv[], MacroContext *context, const Allocator *allocator) {
  Terminal *token = (Terminal *) argv[0];
  if (token->type == enum_IDENTIFIER) {
    uint32_t index = MacroContext_getIdentParamIndex(context, token->value);
    if (index) {
      releaseIdentifier(token->value, allocator);
      allocator->free(token->value);
      token->type = enum_PLACE_HOLDER;
      token->value = (void *) (uint64_t) index - 1;
    }
  }

  Tokens *tokens = Array_new(sizeof(Terminal), enum_TOKEN, allocator);
  Array_append(tokens, token, 1);

  return tokens;
}
