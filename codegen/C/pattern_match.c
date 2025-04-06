/* License
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
 * Filename: pattern_match.c
 * Creator: Yaokai Liu
 * Create Date: 2025-01-19
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "pattern_match.h"
#include "array.h"
#include "encoding.h"
#include "generate.h"

void GenC_gen_pattern_match(Generator *generator, const Machine *machine) {
  ParseContext *context = machine->context;
  GenC_gen_jump_table_dec(generator, machine);
  Array *key_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  Array *state_buffer = Array_new(sizeof(char_t), -1, GContext_getAllocator(context));
  GenC_gen_jump_table_def(generator, machine, key_buffer, state_buffer);
  Array *buffer = CGenerator_getOutputBuffer((CGenerator *) generator, GenC_definitions);
  Array_concat(buffer, key_buffer);
  Array_concat(buffer, state_buffer);
  releasePrimeArray(key_buffer);
  releasePrimeArray(state_buffer);
}
