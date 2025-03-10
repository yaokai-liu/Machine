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
 * Filename: context.h
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_CONTEXT_H
#define MACHINE_CONTEXT_H

#include "stack.h"
#include "target.h"
#include "trie.h"

#define REFER(T) T *

typedef struct MacroContext {
  const Allocator *allocator;
  Array *macroArray;  // Array<Macro>
  Trie *macroTrie;    // Trie<char_t, Macro>
  Stack *call_stack;  // Stack<MacroCall>

  MacroParams *current_params;
  MacroArgs *current_args;
  bool end_parse;
  bool in_parse;
  uint32_t depth;
} MacroContext;

MacroContext *MacroContext_new(const Allocator *allocator);

REFER(Macro) MacroContext_addMacro(MacroContext *context, Macro *macro);
uint32_t MacroContext_getIdentParamIndex(MacroContext *context, Identifier *ident);
void MacroContext_pushCallStack(MacroContext *context, MacroCall *call);
void MacroContext_popCallStack(MacroContext *context, MacroCall *call);
MacroCallFrame *
    MacroContext_makeFrame(MacroContext *context, MacroCallFrame *frame, REFER(Macro) v_macro);

void MacroContext_destroy(MacroContext *context);

typedef void fn_ctx_act(MacroContext *context, void *token);
fn_ctx_act *macro_get_after_stack_action(uint32_t state);
fn_ctx_act *macro_get_after_reduce_action(uint32_t state);

#endif  // MACHINE_CONTEXT_H
