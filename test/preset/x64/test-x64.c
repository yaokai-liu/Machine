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
 * Module Name: test/preset/x64
 * Filename: test-x64.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-06
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "test-x64.h"
#include "x64.h"
#include <stdio.h>

#define test_encoding(bytes, expected_size, instr, ...)                                         \
  do {                                                                                          \
    bool matched = true;                                                                        \
    const uint32_t old_len = Array_length(output_array);                                        \
    uint32_t actual_size = encodingInstr(output_array, instr, __VA_ARGS__, EOI);                \
    if (expected_size != actual_size) {                                                         \
      fprintf(                                                                                  \
          stderr, "incompatible size in %s:%u: expected: %u, actual: %u\n", __FILE__, __LINE__, \
          expected_size, actual_size                                                            \
      );                                                                                        \
      matched = false;                                                                          \
    }                                                                                           \
    const uint8_t * const buffer = Array_first_real(output_array) + old_len;                    \
    for (uint32_t i = 0; i < actual_size && matched; i++) {                                     \
      if (bytes[i] != buffer[i]) {                                                              \
        fprintf(                                                                                \
            stderr, "incompatible byte in %s:%u: expected: 0x%02x, actual: 0x%02x\n", __FILE__, \
            __LINE__, bytes[i], buffer[i]                                                       \
        );                                                                                      \
        matched = false;                                                                        \
        break;                                                                                  \
      }                                                                                         \
    }                                                                                           \
    if (!matched) {                                                                             \
      fprintf(stderr, "expected bytes: ");                                                      \
      for (uint32_t i = 0; i < expected_size; i++) { fprintf(stderr, "%02x ", bytes[i]); }      \
      fprintf(stderr, "\nactual bytes:   ");                                                    \
      for (uint32_t i = 0; i < actual_size; i++) { fprintf(stderr, "%02x ", buffer[i]); }       \
      fprintf(stderr, "\n");                                                                    \
      n_failed++;                                                                               \
    }                                                                                           \
  } while (false)

#define NEW_TEST(test_name) uint32_t test_name(Machine *machine, Array *output_array)

#define add_test(test_name) \
  do { n_failed += test_name(machine, output_array); } while (false)

NEW_TEST(test_primary_instr_r8_r8) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x00, 0xc8};
  test_encoding(bytes1, 2, INSTR_add, REG_al, REG_cl);
  uint8_t bytes2[] = {0x44, 0x00, 0xc0};
  test_encoding(bytes2, 3, INSTR_add, REG_al, REG_r8b);
  uint8_t bytes3[] = {0x41, 0x00, 0xcb};
  test_encoding(bytes3, 3, INSTR_add, REG_r11b, REG_cl);

  // test by varying opcode
  uint8_t bytes4[] = {0x41, 0x28, 0xcb};
  test_encoding(bytes4, 3, INSTR_sub, REG_r11b, REG_cl);
  uint8_t bytes5[] = {0x41, 0x2a, 0xcb};
  test_encoding(bytes5, 3, INSTR_rsub, REG_r11b, REG_cl);
  uint8_t bytes6[] = {0x45, 0x88, 0xea};
  test_encoding(bytes6, 3, INSTR_mov, REG_r10b, REG_r13b);
  uint8_t bytes8[] = {0x41, 0x8a, 0xea};
  test_encoding(bytes8, 3, INSTR_rmov, REG_r10b, REG_bpl);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_rv_rv) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x66, 0x01, 0xf3};
  test_encoding(bytes1, 3, INSTR_add, REG_bx, REG_si);
  uint8_t bytes2[] = {0x66, 0x44, 0x01, 0xca};
  test_encoding(bytes2, 4, INSTR_add, REG_dx, REG_r9w);
  uint8_t bytes3[] = {0x41, 0x01, 0xcb};
  test_encoding(bytes3, 3, INSTR_add, REG_r11d, REG_ecx);
  uint8_t bytes4[] = {0x4d, 0x01, 0xea};
  test_encoding(bytes4, 3, INSTR_add, REG_r10, REG_r13);

  // test by varying opcode
  uint8_t bytes5[] = {0x4d, 0x39, 0xea};
  test_encoding(bytes5, 3, INSTR_cmp, REG_r10, REG_r13);
  uint8_t bytes6[] = {0x4d, 0x89, 0xea};
  test_encoding(bytes6, 3, INSTR_mov, REG_r10, REG_r13);
  uint8_t bytes7[] = {0x4d, 0x3b, 0xea};
  test_encoding(bytes7, 3, INSTR_rcmp, REG_r10, REG_r13);
  uint8_t bytes8[] = {0x49, 0x8b, 0xea};
  test_encoding(bytes8, 3, INSTR_rmov, REG_r10, REG_rbp);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m8_r8) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x40, 0x00, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_REFb(REG_rbx), REG_sil);
  uint8_t bytes2[] = {0x67, 0x00, 0x0a};
  test_encoding(bytes2, 3, INSTR_add, MEM_REFb(REG_edx), REG_cl);
  uint8_t bytes3[] = {0x44, 0x00, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_REFb(REG_rbx), REG_r8b);
  uint8_t bytes4[] = {0x67, 0x44, 0x00, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_REFb(REG_edx), REG_r12b);
  uint8_t bytes5[] = {0x41, 0x00, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_REFb(REG_r11), REG_al);
  uint8_t bytes6[] = {0x67, 0x41, 0x00, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_REFb(REG_r11d), REG_bl);
  uint8_t bytes7[] = {0x45, 0x00, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_REFb(REG_r10), REG_r13b);
  uint8_t bytes8[] = {0x67, 0x45, 0x00, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_REFb(REG_r9d), REG_r13b);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x10, 0x29};
  test_encoding(bytes9, 4, INSTR_adc, MEM_REFb(REG_r9d), REG_r13b);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m16_r16) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x66, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_REFv(REG_rbx), REG_si);
  uint8_t bytes2[] = {0x67, 0x66, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_REFv(REG_edx), REG_cx);
  uint8_t bytes3[] = {0x66, 0x44, 0x01, 0x03};
  test_encoding(bytes3, 4, INSTR_add, MEM_REFv(REG_rbx), REG_r8w);
  uint8_t bytes4[] = {0x67, 0x66, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 5, INSTR_add, MEM_REFv(REG_edx), REG_r12w);
  uint8_t bytes5[] = {0x66, 0x41, 0x01, 0x03};
  test_encoding(bytes5, 4, INSTR_add, MEM_REFv(REG_r11), REG_ax);
  uint8_t bytes6[] = {0x67, 0x66, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 5, INSTR_add, MEM_REFv(REG_r11d), REG_bx);
  uint8_t bytes7[] = {0x66, 0x45, 0x01, 0x2a};
  test_encoding(bytes7, 4, INSTR_add, MEM_REFv(REG_r10), REG_r13w);
  uint8_t bytes8[] = {0x67, 0x66, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 5, INSTR_add, MEM_REFv(REG_r9d), REG_r13w);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x66, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 5, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13w);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m32_r32) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x01, 0x33};
  test_encoding(bytes1, 2, INSTR_add, MEM_REFv(REG_rbx), REG_esi);
  uint8_t bytes2[] = {0x67, 0x01, 0x0a};
  test_encoding(bytes2, 3, INSTR_add, MEM_REFv(REG_edx), REG_ecx);
  uint8_t bytes3[] = {0x44, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_REFv(REG_rbx), REG_r8d);
  uint8_t bytes4[] = {0x67, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_REFv(REG_edx), REG_r12d);
  uint8_t bytes5[] = {0x41, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_REFv(REG_r11), REG_eax);
  uint8_t bytes6[] = {0x67, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_REFv(REG_r11d), REG_ebx);
  uint8_t bytes7[] = {0x45, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_REFv(REG_r10), REG_r13d);
  uint8_t bytes8[] = {0x67, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_REFv(REG_r9d), REG_r13d);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13d);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m64_r64) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x48, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_REFv(REG_rbx), REG_rsi);
  uint8_t bytes2[] = {0x67, 0x48, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_REFv(REG_edx), REG_rcx);
  uint8_t bytes3[] = {0x4c, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_REFv(REG_rbx), REG_r8);
  uint8_t bytes4[] = {0x67, 0x4c, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_REFv(REG_edx), REG_r12);
  uint8_t bytes5[] = {0x49, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_REFv(REG_r11), REG_rax);
  uint8_t bytes6[] = {0x67, 0x49, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_REFv(REG_r11d), REG_rbx);
  uint8_t bytes7[] = {0x4d, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_REFv(REG_r10), REG_r13);
  uint8_t bytes8[] = {0x67, 0x4d, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_REFv(REG_r9d), REG_r13);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x4d, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r8_m8) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x02, 0x33};
  test_encoding(bytes1, 2, INSTR_radd, REG_dl, MEM_REFb(REG_rbx));
  uint8_t bytes2[] = {0x67, 0x02, 0x0a};
  test_encoding(bytes2, 3, INSTR_radd, REG_cl, MEM_REFb(REG_edx));
  uint8_t bytes3[] = {0x44, 0x00, 0x03};
  test_encoding(bytes3, 3, INSTR_radd, REG_r8b, MEM_REFb(REG_rbx));
  uint8_t bytes4[] = {0x67, 0x44, 0x00, 0x22};
  test_encoding(bytes4, 4, INSTR_radd, REG_r12b, MEM_REFb(REG_edx));
  uint8_t bytes5[] = {0x41, 0x00, 0x03};
  test_encoding(bytes5, 3, INSTR_radd, REG_al, MEM_REFb(REG_r11));
  uint8_t bytes6[] = {0x67, 0x41, 0x00, 0x1b};
  test_encoding(bytes6, 4, INSTR_radd, REG_bl, MEM_REFb(REG_r11d));
  uint8_t bytes7[] = {0x45, 0x00, 0x2a};
  test_encoding(bytes7, 3, INSTR_radd, REG_r13b, MEM_REFb(REG_r10));
  uint8_t bytes8[] = {0x67, 0x45, 0x00, 0x29};
  test_encoding(bytes8, 4, INSTR_radd, REG_r13b, MEM_REFb(REG_r9d));

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x10, 0x29};
  test_encoding(bytes9, 4, INSTR_radc, MEM_REFb(REG_r9d), REG_r13b);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_rmov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r16_m16) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x66, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_REFv(REG_rbx), REG_si);
  uint8_t bytes2[] = {0x67, 0x66, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_REFv(REG_edx), REG_cx);
  uint8_t bytes3[] = {0x66, 0x44, 0x01, 0x03};
  test_encoding(bytes3, 4, INSTR_add, MEM_REFv(REG_rbx), REG_r8w);
  uint8_t bytes4[] = {0x67, 0x66, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 5, INSTR_add, MEM_REFv(REG_edx), REG_r12w);
  uint8_t bytes5[] = {0x66, 0x41, 0x01, 0x03};
  test_encoding(bytes5, 4, INSTR_add, MEM_REFv(REG_r11), REG_ax);
  uint8_t bytes6[] = {0x67, 0x66, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 5, INSTR_add, MEM_REFv(REG_r11d), REG_bx);
  uint8_t bytes7[] = {0x66, 0x45, 0x01, 0x2a};
  test_encoding(bytes7, 4, INSTR_add, MEM_REFv(REG_r10), REG_r13w);
  uint8_t bytes8[] = {0x67, 0x66, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 5, INSTR_add, MEM_REFv(REG_r9d), REG_r13w);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x66, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 5, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13w);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r32_m32) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x01, 0x33};
  test_encoding(bytes1, 2, INSTR_add, MEM_REFv(REG_rbx), REG_esi);
  uint8_t bytes2[] = {0x67, 0x01, 0x0a};
  test_encoding(bytes2, 3, INSTR_add, MEM_REFv(REG_edx), REG_ecx);
  uint8_t bytes3[] = {0x44, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_REFv(REG_rbx), REG_r8d);
  uint8_t bytes4[] = {0x67, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_REFv(REG_edx), REG_r12d);
  uint8_t bytes5[] = {0x41, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_REFv(REG_r11), REG_eax);
  uint8_t bytes6[] = {0x67, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_REFv(REG_r11d), REG_ebx);
  uint8_t bytes7[] = {0x45, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_REFv(REG_r10), REG_r13d);
  uint8_t bytes8[] = {0x67, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_REFv(REG_r9d), REG_r13d);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13d);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r64_m64) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x48, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_REFv(REG_rbx), REG_rsi);
  uint8_t bytes2[] = {0x67, 0x48, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_REFv(REG_edx), REG_rcx);
  uint8_t bytes3[] = {0x4c, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_REFv(REG_rbx), REG_r8);
  uint8_t bytes4[] = {0x67, 0x4c, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_REFv(REG_edx), REG_r12);
  uint8_t bytes5[] = {0x49, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_REFv(REG_r11), REG_rax);
  uint8_t bytes6[] = {0x67, 0x49, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_REFv(REG_r11d), REG_rbx);
  uint8_t bytes7[] = {0x4d, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_REFv(REG_r10), REG_r13);
  uint8_t bytes8[] = {0x67, 0x4d, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_REFv(REG_r9d), REG_r13);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x4d, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_REFv(REG_r9d), REG_r13);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_REFv(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

uint32_t test_primary_instructions(Machine *machine, Array *output_array) {
  uint32_t n_failed = 0;
  add_test(test_primary_instr_r8_r8);
  add_test(test_primary_instr_rv_rv);
  add_test(test_primary_instr_m8_r8);
  add_test(test_primary_instr_m16_r16);
  add_test(test_primary_instr_m32_r32);
  add_test(test_primary_instr_m64_r64);
  return n_failed;
}

