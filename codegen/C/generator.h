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
 * Module Name: codegen/C
 * Filename: generator.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GENERATOR_H
#define MACHINE_GENERATOR_H

#include "array.h"
#include "char_t.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Generator {
  const Allocator *allocator;
  const char_t *outname;
  const char_t *headpath;
  const char_t *libpath;
  const char_t *cr_holder;
  const char_t *year;
  Array /*<char_t>*/ *buffers[16];
} Generator;

#endif  // MACHINE_GENERATOR_H
