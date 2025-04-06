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
 * Module Name: template
 * Filename: tokens.gen.h
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENS_H
#define MACHINE_TOKENS_H

#include "char_t.h"
#include <stdint.h>

enum TOKEN_TYPE_ENUM: uint32_t {
  ${enums},
  enum_PLACE_HOLDER,
  MAX_TOTAL_TOKEN,
  enum_BAD_TOKEN = UINT32_MAX
};
extern const char_t *TOKEN_NAMES[];
#endif  // MACHINE_TOKENS_H
