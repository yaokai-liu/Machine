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
 * Module Name:
 * Filename: test.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-21
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/
#define USING_INSTR_DIRECTLY
#include "x64.h"
#include <stdio.h>

int main() {
  Machine *machine = Machine_new(&STDAllocator);
  useMachine(machine);
  Array *output_array = Array_new(sizeof(uint8_t), -1, &STDAllocator);
  uint32_t size = add(output_array, MEM_REFv(REG_rax), REG_cx, EOI);
  //  uint32_t size = test(output_array, MEM_REFb(REG_eax), REG_al, EOI);
  //  uint32_t size = addi(output_array, REG_r8, IMM_Ib(0x1234), EOI);
  //  uint32_t size = addi(output_array, IMM_Ib(0x1234), EOI);
  //  uint32_t size = sub(output_array, MEM_REFv(REG_ecx), REG_ax, EOI);
  //  uint32_t size = movabs(output_array, REG_ax, IMM_Iw(0x123456789abcdef), EOI);
  //  uint32_t size = movi(output_array, REG_r8, IMM_Id(0x123456789abcdef), EOI);
  //  uint32_t size = nop(output_array, EOI);
  //  uint32_t size = cmpsq_e(output_array, EOI);
  //  uint32_t size = testi(output_array, REG_eax, IMM_Id(0x1234567), EOI);
  //  uint32_t size = lea(output_array, REG_ecx, MEM_REFv(REG_rax), EOI);
  //  uint32_t size = addi(output_array, IMM_Ib(0x12345678), EOI);
  //  uint32_t size = push(output_array, REG_r12, EOI);
  //  uint32_t size = ret(output_array, IMM_Iw(0x1234), EOI);
  uint8_t *buffer = Array_first_real(output_array);
  for (uint32_t i = 0; i < Array_length(output_array); i++) { printf("%02x ", buffer[i]); }
  printf("\nsize=%u\n", size);
  return size == 0;
}
