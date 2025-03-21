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
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2025-03-09
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENIZE_TARGET_H
#define MACHINE_TOKENIZE_TARGET_H

#include "array.h"
#include "generated/tokens.gen.h"
#include "terminal.h"

typedef void MacroEntry;
typedef struct Macro Macro;
typedef struct MacroArg MacroArg;
typedef Array MacroArgs;  // Array<MacroArg>
typedef struct MacroCall MacroCall;
typedef struct Array MacroParams;  // Array<REFER(Identifier)>
typedef Array Tokens;              // Array<Token>

struct MacroArg {
  uint32_t type;
  void *target;
};

struct MacroCall {
  Identifier *name;
  MacroArgs *args;
};

struct Macro {
  Identifier *name;
  MacroParams *params;
  Tokens *tokens;
};

typedef struct MacroCallFrame {
  const Tokens *tokens;
  uint32_t index;
  MacroArgs *args;
  TokenPos position;
} MacroCallFrame;

typedef Array Concat; // Array<Token>

void releaseMacro(Macro *macro, const Allocator *allocator);
void releaseMacroArg(MacroArg *arg, const Allocator *allocator);
void releaseMacroToken(Token *token, const Allocator *allocator);

#endif  // MACHINE_TOKENIZE_TARGET_H
