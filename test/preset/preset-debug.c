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
  x64Machine *machine = x64Machine_new(&STDAllocator);
  useMachine(machine);
  Array *output_array = Array_new(sizeof(uint8_t), -1, &STDAllocator);
#define PROG_INSTR_BUFFER output_array
  uint32_t size = add(MEM_SIBb(REG_rbp, REG_rbx, 2), REG_bl);
  //  uint32_t size = test(MEM_PTRb(REG_eax), REG_al);
  //  uint32_t size = addi(REG_r8, IMM_Ib(0x1234));
  //  uint32_t size = addi(IMM_Ib(0x1234));
  //  uint32_t size = sub(MEM_PTR(REG_ecx), REG_ax);
  //  uint32_t size = movabs(REG_ax, IMM_Iw(0x123456789abcdef));
  //  uint32_t size = movi(REG_r8, IMM_Id(0x123456789abcdef));
  //  uint32_t size = nop(EOI);
  //  uint32_t size = cmpsq_e(EOI);
  //  uint32_t size = testi(REG_eax, IMM_Id(0x1234567));
  //  uint32_t size = lea(REG_ecx, MEM_PTR(REG_rax));
  //  uint32_t size = addi(IMM_Ib(0x12345678));
  //  uint32_t size = push(REG_r12);
  //  uint32_t size = ret(IMM_Iw(0x1234));
  uint8_t *buffer = Array_first_real(output_array);
  for (uint32_t i = 0; i < Array_length(output_array); i++) { printf("%02x ", buffer[i]); }
  printf("\nsize=%u\n", size);
  return size == 0;
}
