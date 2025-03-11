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
 * Module Name: codegen
 * Filename: generator.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-20
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "generator.h"

Generator *Generator_new(const Array *ident_array, const Allocator *allocator) {
  Generator *generator = allocator->calloc(1, sizeof(Generator));
  generator->ident_array = ident_array;
  generator->allocator = allocator;
  return generator;
}
void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *headpath, const char_t *libpath,
    const char_t *cr_holder, const char_t *year
) {
  generator->outname = outname;
  generator->headpath = headpath;
  generator->libpath = libpath;
  generator->cr_holder = cr_holder;
  generator->year = year;
}

inline Array *Generator_getOutputBuffer(Generator *generator, uint32_t index) {
  if (index >= 16) { return nullptr; }
  if (!generator->buffers[index]) {
    generator->buffers[index] = Array_new(sizeof(char_t), INT32_MAX, generator->allocator);
  }
  return generator->buffers[index];
}

void Generator_destroy(Generator *generator) {
  for (uint32_t i = 0; i < 16; i++) {
    if (generator->buffers[i]) { releasePrimeArray(generator->buffers[i]); }
  }
  generator->allocator->free(generator);
}
