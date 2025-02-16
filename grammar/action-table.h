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
 * Filename: action-table.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ACTION_TABLE_H
#define MACHINE_ACTION_TABLE_H

#include <stdint.h>

typedef struct state state;
struct grammar_action {
  enum : uint8_t {
    reject = 0,
    stack = 1,
    reduce = 2
  } action      : 4;
  uint8_t count : 4;
  uint8_t type;
  const int16_t offset;
};

const struct grammar_action *getAction(uint32_t index, uint32_t ahead);

int32_t jump(uint32_t index, uint32_t current);

uint32_t stateCurrentTokenType(int32_t state);

#endif  // MACHINE_ACTION_TABLE_H
