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
 * Filename: parse.h
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_PARSE_H
#define MACHINE_PARSE_H

#include "context.h"
#include "target.h"
#include "tokenize/tokenizer.h"

extern fn_reduce * const MACHINE_PRODUCTS[];

Machine *parse(Tokenizer *tokenizer, const char_t **err_msg, const Allocator *allocator);

Machine *failed_to_get_next_state(
    Stack *state_stack, Stack *token_stack, Token *token, const Allocator *allocator
);

Machine *failed_to_produce(
    Stack *state_stack, Stack *token_stack, Token args[], uint32_t argc, const Allocator *allocator
);

Machine *clean_parse_stack(Stack *state_stack, Stack *token_stack, const Allocator *allocator);

#endif  // MACHINE_PARSE_H
