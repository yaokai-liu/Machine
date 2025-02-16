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
 * Module Name: grammar
 * Filename: tokenize.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-31
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENIZE_H
#define MACHINE_TOKENIZE_H

#include "char_t.h"
#include "terminal.h"
#include <stdint.h>

const Terminal *tokenize(
    const char_t *input, uint32_t *cost, uint32_t *n_tokens, uint32_t *lineno, uint32_t *column,
    const Allocator *allocator
);

const char_t *get_name(uint16_t type);

#endif  // MACHINE_TOKENIZE_H
