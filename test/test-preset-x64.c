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
 * Module Name: test
 * Filename: test-preset.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-06
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "test-x64.h"

int main() {
  uint32_t n_failed = 0;
  Machine *machine = Machine_new(&STDAllocator);
  Array *output_array = Array_new(sizeof(uint8_t), -1, &STDAllocator);

  useMachine(machine);
  n_failed += test_add(machine, output_array);

  Array_destroy(output_array);
  Machine_destroy(machine);
  return n_failed ? -1 : 0;
}
