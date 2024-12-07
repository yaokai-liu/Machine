/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: test-number.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-14
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "allocator.h"
#include "char_t.h"
#include "terminal.h"
#include "tokenize.h"
#include "tokens.gen.h"
#include <check.h>
#include <stdint.h>

#define lenof(str_literal) ((sizeof str_literal) - 1)

START_TEST(test_NUMBER_r16) {
  const char_t *string = "0x123456789abcdef0";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("0x123456789abcdef0"));
  ck_assert_uint_eq(n_tokens, 2);
  ck_assert_ptr_ne(terminals, nullptr);
  ck_assert_uint_eq(terminals[0].type, enum_NUMBER);
  ck_assert_uint_eq(terminals[0].lineno, 0);
  ck_assert_uint_eq(terminals[0].column, 0);
  ck_assert_uint_eq(terminals[0].length, lenof("0x123456789abcdef0"));
  ck_assert_uint_eq((uint64_t) terminals[0].value, 0x123456789abcdef0LL);
  ck_assert_str_eq(get_name(terminals[0].type), "NUMBER");
  ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
  ck_assert_ptr_eq(terminals[1].value, nullptr);
  ck_assert_uint_eq(terminals[1].lineno, 0);
  ck_assert_uint_eq(terminals[1].column, lenof("0x123456789abcdef0"));
  ck_assert_uint_eq(terminals[1].length, 0);
  ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
  STDAllocator.free((void *) terminals);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0x123456789abcdef0"));
}
END_TEST

START_TEST(test_NUMBER_r10) {
  const char_t *string = "1234567890";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("1234567890"));
  ck_assert_uint_eq(n_tokens, 2);
  ck_assert_ptr_ne(terminals, nullptr);
  ck_assert_uint_eq(terminals[0].type, enum_NUMBER);
  ck_assert_uint_eq(terminals[0].lineno, 0);
  ck_assert_uint_eq(terminals[0].column, 0);
  ck_assert_uint_eq(terminals[0].length, lenof("1234567890"));
  ck_assert_uint_eq((uint64_t) terminals[0].value, 1234567890);
  ck_assert_str_eq(get_name(terminals[0].type), "NUMBER");
  ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
  ck_assert_ptr_eq(terminals[1].value, nullptr);
  ck_assert_uint_eq(terminals[1].lineno, 0);
  ck_assert_uint_eq(terminals[1].column, lenof("1234567890"));
  ck_assert_uint_eq(terminals[1].length, 0);
  ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
  STDAllocator.free((void *) terminals);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("1234567890"));
}
END_TEST

START_TEST(test_NUMBER_r8) {
  const char_t *string = "0o12345670";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("0o12345670"));
  ck_assert_uint_eq(n_tokens, 2);
  ck_assert_ptr_ne(terminals, nullptr);
  ck_assert_uint_eq(terminals[0].type, enum_NUMBER);
  ck_assert_uint_eq((uint64_t) terminals[0].value, 012345670);
  ck_assert_uint_eq(terminals[0].lineno, 0);
  ck_assert_uint_eq(terminals[0].column, 0);
  ck_assert_uint_eq(terminals[0].length, lenof("0o12345670"));
  ck_assert_str_eq(get_name(terminals[0].type), "NUMBER");
  ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
  ck_assert_ptr_eq(terminals[1].value, nullptr);
  ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
  ck_assert_uint_eq(terminals[1].lineno, 0);
  ck_assert_uint_eq(terminals[1].column, lenof("0o12345670"));
  ck_assert_uint_eq(terminals[1].length, 0);
  STDAllocator.free((void *) terminals);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0o12345670"));
}
END_TEST

START_TEST(test_NUMBER_r2) {
  const char_t *string = "0b101010101";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("0b101010101"));
  ck_assert_uint_eq(n_tokens, 2);
  ck_assert_ptr_ne(terminals, nullptr);
  ck_assert_uint_eq(terminals[0].type, enum_NUMBER);
  ck_assert_uint_eq((uint64_t) terminals[0].value, 0b101010101);
  ck_assert_uint_eq(terminals[0].lineno, 0);
  ck_assert_uint_eq(terminals[0].column, 0);
  ck_assert_uint_eq(terminals[0].length, lenof("0b101010101"));
  ck_assert_str_eq(get_name(terminals[0].type), "NUMBER");
  ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
  ck_assert_ptr_eq(terminals[1].value, nullptr);
  ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
  ck_assert_uint_eq(terminals[1].lineno, 0);
  ck_assert_uint_eq(terminals[1].column, lenof("0b101010101"));
  ck_assert_uint_eq(terminals[1].length, 0);
  STDAllocator.free((void *) terminals);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0b101010101"));
}
END_TEST

START_TEST(test_NUMBER_r16_failed) {
  const char_t *string = "0x123456gs";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, 0);
  ck_assert_uint_eq(n_tokens, 0);
  ck_assert_ptr_eq(terminals, nullptr);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0x123456"));
}
END_TEST

START_TEST(test_NUMBER_r10_failed) {
  const char_t *string = "12345abc";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, 0);
  ck_assert_uint_eq(n_tokens, 0);
  ck_assert_ptr_eq(terminals, nullptr);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("12345"));
}
END_TEST

START_TEST(test_NUMBER_r8_failed) {
  const char_t *string = "0o123456789a";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, 0);
  ck_assert_uint_eq(n_tokens, 0);
  ck_assert_ptr_eq(terminals, nullptr);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0o1234567"));
}
END_TEST

START_TEST(test_NUMBER_r2_failed) {
  const char_t *string = "0b101236ab";
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, 0);
  ck_assert_uint_eq(n_tokens, 0);
  ck_assert_ptr_eq(terminals, nullptr);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("0b101"));
}
END_TEST

Suite *number_suite() {
  Suite *suite = suite_create("Numbers");
  TCase *tc_numbers = tcase_create("numbers");
  tcase_add_test(tc_numbers, test_NUMBER_r16);
  tcase_add_test(tc_numbers, test_NUMBER_r10);
  tcase_add_test(tc_numbers, test_NUMBER_r8);
  tcase_add_test(tc_numbers, test_NUMBER_r2);
  tcase_add_test(tc_numbers, test_NUMBER_r16_failed);
  tcase_add_test(tc_numbers, test_NUMBER_r10_failed);
  tcase_add_test(tc_numbers, test_NUMBER_r8_failed);
  tcase_add_test(tc_numbers, test_NUMBER_r2_failed);
  suite_add_tcase(suite, tc_numbers);
  return suite;
}
