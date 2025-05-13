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

#define testing_assert(expr)                                           \
  do {                                                                 \
    if (!(expr)) {                                                     \
      fprintf(stderr, "assert failed in %s:%u\n", __FILE__, __LINE__); \
      n_failed++;                                                      \
    }                                                                  \
  } while (false)

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
    const uint8_t * const buffer = ((uint8_t *) Array_first_real(output_array)) + old_len;      \
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

#define NEW_TEST(test_name) uint32_t test_name(x64Machine *machine, Array *output_array)

#define add_test(test_name) \
  do { n_failed += test_name(machine, output_array); } while (false)

NEW_TEST(test_memory_model_REF) {
  uint32_t n_failed = 0;
  testing_assert(nullptr == MEM_PTR(REG_rsp));
  testing_assert(nullptr == MEM_PTR(REG_rbp));
  testing_assert(nullptr == MEM_PTR(REG_esp));
  testing_assert(nullptr == MEM_PTR(REG_ebp));

  testing_assert(nullptr == MEM_PTR(REG_ebp));
  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

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
  uint8_t bytes9[] = {0x49, 0x87, 0xc0};
  test_encoding(bytes9, 3, INSTR_xchg, REG_r8, REG_rax);
  uint8_t bytes10[] = {0x48, 0x93};
  test_encoding(bytes10, 2, INSTR_xchgExtend, REG_rbx, REG_rax);
  uint8_t bytes11[] = {0x49, 0x95};
  test_encoding(bytes11, 2, INSTR_xchgExtend, REG_r13, REG_rax);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m8_r8) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x40, 0x00, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_PTR(REG_rbx), REG_sil);
  uint8_t bytes2[] = {0x67, 0x00, 0x0a};
  test_encoding(bytes2, 3, INSTR_add, MEM_PTR(REG_edx), REG_cl);
  uint8_t bytes3[] = {0x44, 0x00, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_PTR(REG_rbx), REG_r8b);
  uint8_t bytes4[] = {0x67, 0x44, 0x00, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_PTR(REG_edx), REG_r12b);
  uint8_t bytes5[] = {0x41, 0x00, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_PTR(REG_r11), REG_al);
  uint8_t bytes6[] = {0x67, 0x41, 0x00, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_PTR(REG_r11d), REG_bl);
  uint8_t bytes7[] = {0x45, 0x00, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_PTR(REG_r10), REG_r13b);
  uint8_t bytes8[] = {0x67, 0x45, 0x00, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_PTR(REG_r9d), REG_r13b);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x10, 0x29};
  test_encoding(bytes9, 4, INSTR_adc, MEM_PTR(REG_r9d), REG_r13b);
  uint8_t bytes10[] = {0x67, 0x45, 0x88, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_PTR(REG_r9d), REG_r13b);
  uint8_t bytes11[] = {0x67, 0x45, 0x86, 0x29};
  test_encoding(bytes11, 4, INSTR_xchg, MEM_PTR(REG_r9d), REG_r13b);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m16_r16) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x66, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_PTR(REG_rbx), REG_si);
  uint8_t bytes2[] = {0x67, 0x66, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_PTR(REG_edx), REG_cx);
  uint8_t bytes3[] = {0x66, 0x44, 0x01, 0x03};
  test_encoding(bytes3, 4, INSTR_add, MEM_PTR(REG_rbx), REG_r8w);
  uint8_t bytes4[] = {0x67, 0x66, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 5, INSTR_add, MEM_PTR(REG_edx), REG_r12w);
  uint8_t bytes5[] = {0x66, 0x41, 0x01, 0x03};
  test_encoding(bytes5, 4, INSTR_add, MEM_PTR(REG_r11), REG_ax);
  uint8_t bytes6[] = {0x67, 0x66, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 5, INSTR_add, MEM_PTR(REG_r11d), REG_bx);
  uint8_t bytes7[] = {0x66, 0x45, 0x01, 0x2a};
  test_encoding(bytes7, 4, INSTR_add, MEM_PTR(REG_r10), REG_r13w);
  uint8_t bytes8[] = {0x67, 0x66, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 5, INSTR_add, MEM_PTR(REG_r9d), REG_r13w);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x66, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 5, INSTR_sbb, MEM_PTR(REG_r9d), REG_r13w);
  uint8_t bytes10[] = {0x67, 0x66, 0x45, 0x89, 0x29};
  test_encoding(bytes10, 5, INSTR_mov, MEM_PTR(REG_r9d), REG_r13w);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m32_r32) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x01, 0x33};
  test_encoding(bytes1, 2, INSTR_add, MEM_PTR(REG_rbx), REG_esi);
  uint8_t bytes2[] = {0x67, 0x01, 0x0a};
  test_encoding(bytes2, 3, INSTR_add, MEM_PTR(REG_edx), REG_ecx);
  uint8_t bytes3[] = {0x44, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_PTR(REG_rbx), REG_r8d);
  uint8_t bytes4[] = {0x67, 0x44, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_PTR(REG_edx), REG_r12d);
  uint8_t bytes5[] = {0x41, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_PTR(REG_r11), REG_eax);
  uint8_t bytes6[] = {0x67, 0x41, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_PTR(REG_r11d), REG_ebx);
  uint8_t bytes7[] = {0x45, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_PTR(REG_r10), REG_r13d);
  uint8_t bytes8[] = {0x67, 0x45, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_PTR(REG_r9d), REG_r13d);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_PTR(REG_r9d), REG_r13d);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_PTR(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_m64_r64) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x48, 0x01, 0x33};
  test_encoding(bytes1, 3, INSTR_add, MEM_PTR(REG_rbx), REG_rsi);
  uint8_t bytes2[] = {0x67, 0x48, 0x01, 0x0a};
  test_encoding(bytes2, 4, INSTR_add, MEM_PTR(REG_edx), REG_rcx);
  uint8_t bytes3[] = {0x4c, 0x01, 0x03};
  test_encoding(bytes3, 3, INSTR_add, MEM_PTR(REG_rbx), REG_r8);
  uint8_t bytes4[] = {0x67, 0x4c, 0x01, 0x22};
  test_encoding(bytes4, 4, INSTR_add, MEM_PTR(REG_edx), REG_r12);
  uint8_t bytes5[] = {0x49, 0x01, 0x03};
  test_encoding(bytes5, 3, INSTR_add, MEM_PTR(REG_r11), REG_rax);
  uint8_t bytes6[] = {0x67, 0x49, 0x01, 0x1b};
  test_encoding(bytes6, 4, INSTR_add, MEM_PTR(REG_r11d), REG_rbx);
  uint8_t bytes7[] = {0x4d, 0x01, 0x2a};
  test_encoding(bytes7, 3, INSTR_add, MEM_PTR(REG_r10), REG_r13);
  uint8_t bytes8[] = {0x67, 0x4d, 0x01, 0x29};
  test_encoding(bytes8, 4, INSTR_add, MEM_PTR(REG_r9d), REG_r13);

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x4d, 0x19, 0x29};
  test_encoding(bytes9, 4, INSTR_sbb, MEM_PTR(REG_r9d), REG_r13);
  uint8_t bytes10[] = {0x67, 0x4d, 0x89, 0x29};
  test_encoding(bytes10, 4, INSTR_mov, MEM_PTR(REG_r9d), REG_r13);
  uint8_t bytes11[] = {0x67, 0x4d, 0x87, 0x29};
  test_encoding(bytes11, 4, INSTR_xchg, MEM_PTR(REG_r9d), REG_r13);

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r8_m8) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x02, 0x13};
  test_encoding(bytes1, 2, INSTR_radd, REG_dl, MEM_PTR(REG_rbx));
  uint8_t bytes1h[] = {0x02, 0x33};
  test_encoding(bytes1h, 2, INSTR_radd, REG_dh, MEM_PTR(REG_rbx));
  uint8_t bytes2[] = {0x67, 0x02, 0x0a};
  test_encoding(bytes2, 3, INSTR_radd, REG_cl, MEM_PTR(REG_edx));
  uint8_t bytes3[] = {0x44, 0x02, 0x03};
  test_encoding(bytes3, 3, INSTR_radd, REG_r8b, MEM_PTR(REG_rbx));
  uint8_t bytes4[] = {0x67, 0x44, 0x02, 0x22};
  test_encoding(bytes4, 4, INSTR_radd, REG_r12b, MEM_PTR(REG_edx));
  uint8_t bytes5[] = {0x41, 0x02, 0x03};
  test_encoding(bytes5, 3, INSTR_radd, REG_al, MEM_PTR(REG_r11));
  uint8_t bytes6[] = {0x67, 0x41, 0x02, 0x1b};
  test_encoding(bytes6, 4, INSTR_radd, REG_bl, MEM_PTR(REG_r11d));
  uint8_t bytes7[] = {0x45, 0x02, 0x2a};
  test_encoding(bytes7, 3, INSTR_radd, REG_r13b, MEM_PTR(REG_r10));
  uint8_t bytes8[] = {0x67, 0x45, 0x02, 0x29};
  test_encoding(bytes8, 4, INSTR_radd, REG_r13b, MEM_PTR(REG_r9d));

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x12, 0x0b};
  test_encoding(bytes9, 4, INSTR_radc, REG_r9b, MEM_PTR(REG_r11d));
  uint8_t bytes10[] = {0x45, 0x8a, 0x0b};
  test_encoding(bytes10, 3, INSTR_rmov, REG_r9b, MEM_PTR(REG_r11));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r16_m16) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x66, 0x03, 0x1e};
  test_encoding(bytes1, 3, INSTR_radd, REG_bx, MEM_PTR(REG_rsi));
  uint8_t bytes2[] = {0x67, 0x66, 0x03, 0x11};
  test_encoding(bytes2, 4, INSTR_radd, REG_dx, MEM_PTR(REG_ecx));
  uint8_t bytes3[] = {0x66, 0x41, 0x03, 0x18};
  test_encoding(bytes3, 4, INSTR_radd, REG_bx, MEM_PTR(REG_r8));
  uint8_t bytes4[] = {0x67, 0x66, 0x41, 0x03, 0x13};
  test_encoding(bytes4, 5, INSTR_radd, REG_dx, MEM_PTR(REG_r11d));
  uint8_t bytes5[] = {0x66, 0x44, 0x03, 0x18};
  test_encoding(bytes5, 4, INSTR_radd, REG_r11w, MEM_PTR(REG_rax));
  uint8_t bytes6[] = {0x67, 0x66, 0x44, 0x03, 0x1b};
  test_encoding(bytes6, 5, INSTR_radd, REG_r11w, MEM_PTR(REG_ebx));
  uint8_t bytes7[] = {0x66, 0x45, 0x03, 0x16};
  test_encoding(bytes7, 4, INSTR_radd, REG_r10w, MEM_PTR(REG_r14));
  uint8_t bytes8[] = {0x67, 0x66, 0x45, 0x03, 0x0e};
  test_encoding(bytes8, 5, INSTR_radd, REG_r9w, MEM_PTR(REG_r14d));

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x66, 0x45, 0x1b, 0x0e};
  test_encoding(bytes9, 5, INSTR_rsbb, REG_r9w, MEM_PTR(REG_r14d));
  uint8_t bytes10[] = {0x67, 0x66, 0x45, 0x8b, 0x0e};
  test_encoding(bytes10, 5, INSTR_rmov, REG_r9w, MEM_PTR(REG_r14d));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r32_m32) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x03, 0x1e};
  test_encoding(bytes1, 2, INSTR_radd, REG_ebx, MEM_PTR(REG_rsi));
  uint8_t bytes2[] = {0x67, 0x03, 0x11};
  test_encoding(bytes2, 3, INSTR_radd, REG_edx, MEM_PTR(REG_ecx));
  uint8_t bytes3[] = {0x41, 0x03, 0x18};
  test_encoding(bytes3, 3, INSTR_radd, REG_ebx, MEM_PTR(REG_r8));
  uint8_t bytes4[] = {0x67, 0x41, 0x03, 0x13};
  test_encoding(bytes4, 4, INSTR_radd, REG_edx, MEM_PTR(REG_r11d));
  uint8_t bytes5[] = {0x44, 0x03, 0x18};
  test_encoding(bytes5, 3, INSTR_radd, REG_r11d, MEM_PTR(REG_rax));
  uint8_t bytes6[] = {0x67, 0x44, 0x03, 0x1b};
  test_encoding(bytes6, 4, INSTR_radd, REG_r11d, MEM_PTR(REG_ebx));
  uint8_t bytes7[] = {0x45, 0x03, 0x16};
  test_encoding(bytes7, 3, INSTR_radd, REG_r10d, MEM_PTR(REG_r14));
  uint8_t bytes8[] = {0x67, 0x45, 0x03, 0x0e};
  test_encoding(bytes8, 4, INSTR_radd, REG_r9d, MEM_PTR(REG_r14d));

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x45, 0x1b, 0x0e};
  test_encoding(bytes9, 4, INSTR_rsbb, REG_r9d, MEM_PTR(REG_r14d));
  uint8_t bytes10[] = {0x67, 0x45, 0x8b, 0x0e};
  test_encoding(bytes10, 4, INSTR_rmov, REG_r9d, MEM_PTR(REG_r14d));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_r64_m64) {
  uint32_t n_failed = 0;
  // test by varying operands
  uint8_t bytes1[] = {0x48, 0x03, 0x1e};
  test_encoding(bytes1, 3, INSTR_radd, REG_rbx, MEM_PTR(REG_rsi));
  uint8_t bytes2[] = {0x67, 0x48, 0x03, 0x11};
  test_encoding(bytes2, 4, INSTR_radd, REG_rdx, MEM_PTR(REG_ecx));
  uint8_t bytes3[] = {0x49, 0x03, 0x18};
  test_encoding(bytes3, 3, INSTR_radd, REG_rbx, MEM_PTR(REG_r8));
  uint8_t bytes4[] = {0x67, 0x49, 0x03, 0x13};
  test_encoding(bytes4, 4, INSTR_radd, REG_rdx, MEM_PTR(REG_r11d));
  uint8_t bytes5[] = {0x4c, 0x03, 0x18};
  test_encoding(bytes5, 3, INSTR_radd, REG_r11, MEM_PTR(REG_rax));
  uint8_t bytes6[] = {0x67, 0x4c, 0x03, 0x1b};
  test_encoding(bytes6, 4, INSTR_radd, REG_r11, MEM_PTR(REG_ebx));
  uint8_t bytes7[] = {0x4d, 0x03, 0x16};
  test_encoding(bytes7, 3, INSTR_radd, REG_r10, MEM_PTR(REG_r14));
  uint8_t bytes8[] = {0x67, 0x4d, 0x03, 0x0e};
  test_encoding(bytes8, 4, INSTR_radd, REG_r9, MEM_PTR(REG_r14d));

  // test by varying opcode
  uint8_t bytes9[] = {0x67, 0x4d, 0x1b, 0x0e};
  test_encoding(bytes9, 4, INSTR_rsbb, REG_r9, MEM_PTR(REG_r14d));
  uint8_t bytes10[] = {0x67, 0x4d, 0x8b, 0x0e};
  test_encoding(bytes10, 4, INSTR_rmov, REG_r9, MEM_PTR(REG_r14d));
  uint8_t bytes11[] = {0x4d, 0x8d, 0x0e};
  test_encoding(bytes11, 3, INSTR_lea, REG_r9, MEM_PTR(REG_r14));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_primary_instr_imm_ax_iz) {
  uint32_t n_failed = 0;

  // test by varying operands
  uint8_t bytes1[] = {0x04, 0x12};
  test_encoding(bytes1, 2, INSTR_addi, IMM_Ib(0x12));
  uint8_t bytes2[] = {0x66, 0x05, 0x12, 0x34};
  test_encoding(bytes2, 4, INSTR_addi, IMM_Iw(0x3412));
  uint8_t bytes3[] = {0x05, 0x12, 0x34, 0x56, 0x78};
  test_encoding(bytes3, 5, INSTR_addi, IMM_Id(0x78563412));
  uint8_t bytes4[] = {0x48, 0x05, 0x12, 0x34, 0x56, 0x78};
  test_encoding(bytes4, 6, INSTR_addi, IMM_Ird(0x78563412));
  // test by varying opcode
  uint8_t bytes5[] = {0x14, 0x12};
  test_encoding(bytes5, 2, INSTR_adci, IMM_Ib(0x12));
  uint8_t bytes6[] = {0x66, 0x15, 0x12, 0x34};
  test_encoding(bytes6, 4, INSTR_adci, IMM_Iw(0x3412));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

NEW_TEST(test_other_instructions) {
  uint32_t n_failed = 0;

  uint8_t bytes1[] = {0x90};
  test_encoding(bytes1, 1, INSTR_nop, EOI);
  uint8_t bytes2[] = {0xc3};
  test_encoding(bytes2, 1, INSTR_ret, EOI);
  uint8_t bytes3[] = {0xc2, 0x44, 0x33};                    // little endian byte order
  test_encoding(bytes3, 3, INSTR_ret, IMM_Iw(0x11223344));  // immediate auto cut off
  uint8_t bytes4[] = {0x70, 0x11};
  test_encoding(bytes4, 2, INSTR_jo, IMM_Ib(0x11));

  if (!n_failed) { fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__); }
  return n_failed;
}

uint32_t test_primary_instructions(x64Machine *machine, Array *output_array) {
  uint32_t n_failed = 0;
  add_test(test_memory_model_REF);
  add_test(test_primary_instr_r8_r8);
  add_test(test_primary_instr_rv_rv);
  add_test(test_primary_instr_m8_r8);
  add_test(test_primary_instr_m16_r16);
  add_test(test_primary_instr_m32_r32);
  add_test(test_primary_instr_m64_r64);
  add_test(test_primary_instr_r8_m8);
  add_test(test_primary_instr_r16_m16);
  add_test(test_primary_instr_r32_m32);
  add_test(test_primary_instr_r64_m64);
  add_test(test_primary_instr_imm_ax_iz);
  add_test(test_other_instructions);
  return n_failed;
}

