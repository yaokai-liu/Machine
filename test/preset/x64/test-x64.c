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

#define test_encoding(bytes, expected_size, instr, ...)                                     \
  do {                                                                                      \
    bool matched = true;                                                                    \
    const uint32_t old_len = Array_length(output_array);                                    \
    uint32_t actual_size = encodingInstr(output_array, instr, __VA_ARGS__, EOI);            \
    if (expected_size != actual_size) {                                                     \
      fprintf(                                                                              \
          stderr, "incompatible size in %s:%u: expected: %02x, actual: %02x\n", __FILE__,   \
          __LINE__, expected_size, actual_size                                              \
      );                                                                                    \
      matched = false;                                                                      \
    }                                                                                       \
    const uint8_t * const buffer = Array_first_real(output_array) + old_len;                \
    for (uint32_t i = 0; i < actual_size && matched; i++) {                                 \
      if (bytes[i] != buffer[i]) {                                                          \
        fprintf(                                                                            \
            stderr, "incompatible byte in %s:%u: expected: %02x, actual: %02x\n", __FILE__, \
            __LINE__, bytes[i], buffer[i]                                                   \
        );                                                                                  \
        matched = false;                                                                    \
        break;                                                                              \
      }                                                                                     \
    }                                                                                       \
    if (!matched) {                                                                         \
      fprintf(stderr, "expected bytes: ");                                                  \
      for (uint32_t i = 0; i < expected_size; i++) { fprintf(stderr, "%02x ", bytes[i]); }  \
      fprintf(stderr, "\nactual bytes:   ");                                                \
      for (uint32_t i = 0; i < actual_size; i++) { fprintf(stderr, "%02x ", buffer[i]); }   \
      fprintf(stderr, "\n");                                                                \
    }                                                                                       \
  } while (false)

#define NEW_TEST(test_name) uint32_t test_name(Machine *machine, Array *output_array)

#define add_test(test_name) \
  do { n_failed += test_name(machine, output_array); } while (false)

NEW_TEST(test_add_r8_r8) {
  uint8_t bytes1[] = {0x00, 0xc8};
  test_encoding(bytes1, 2, INSTR_add, REG_al, REG_cl);
  uint8_t bytes2[] = {0x44, 0x00, 0xc0};
  test_encoding(bytes2, 3, INSTR_add, REG_al, REG_r8b);
  uint8_t bytes3[] = {0x41, 0x00, 0xcb};
  test_encoding(bytes3, 3, INSTR_add, REG_r11b, REG_cl);

  fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__);
  return 0;
}

NEW_TEST(test_add_rv_rv) {
  uint8_t bytes1[] = {0x66, 0x01, 0xf3};
  test_encoding(bytes1, 3, INSTR_add, REG_bx, REG_si);
  uint8_t bytes2[] = {0x66, 0x44, 0x01, 0xca};
  test_encoding(bytes2, 4, INSTR_add, REG_dx, REG_r9w);
  uint8_t bytes3[] = {0x41, 0x01, 0xcb};
  test_encoding(bytes3, 3, INSTR_add, REG_r11d, REG_ecx);
  uint8_t bytes4[] = {0x4d, 0x01, 0xea};
  test_encoding(bytes4, 3, INSTR_add, REG_r10, REG_r13);

  fprintf(stdout, "test for '%s' passed.\n", __FUNCTION__);
  return 0;
}

uint32_t test_add(Machine *machine, Array *output_array) {
  uint32_t n_failed = 0;
  add_test(test_add_r8_r8);
  add_test(test_add_rv_rv);
  return n_failed;
}

