/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: test-keywords.c
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

#define add_test_for(_name, _value)                                          \
  START_TEST(test_##_name) {                                                 \
    char_t *string = _value;                                                 \
    uint32_t cost = 0, n_tokens = 0;                                         \
    uint32_t lineno = 0, column = 0;                                         \
    const Terminal *terminals =                                              \
        tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator); \
    ck_assert_uint_eq(cost, lenof(_value));                                  \
    ck_assert_uint_eq(n_tokens, 2);                                          \
    ck_assert_ptr_ne(terminals, nullptr);                                    \
    ck_assert_uint_eq(terminals[0].type, enum_##_name);                      \
    ck_assert_ptr_eq(terminals[0].value, 0);                                 \
    ck_assert_uint_eq(terminals[0].lineno, 0);                               \
    ck_assert_uint_eq(terminals[0].column, 0);                               \
    ck_assert_uint_eq(terminals[0].length, lenof(_value));                   \
    ck_assert_str_eq(get_name(terminals[0].type), string_t(#_name));         \
    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);                   \
    ck_assert_ptr_eq(terminals[1].value, 0);                                 \
    ck_assert_uint_eq(terminals[1].lineno, 0);                               \
    ck_assert_uint_eq(terminals[1].column, lenof(_value));                   \
    ck_assert_uint_eq(terminals[1].length, 0);                               \
    ck_assert_str_eq(get_name(terminals[1].type), string_t("TERMINATOR"));   \
    STDAllocator.free((void *) terminals);                                   \
    ck_assert_uint_eq(lineno, 0);                                            \
    ck_assert_uint_eq(column, lenof(_value));                                \
  }                                                                          \
  END_TEST

add_test_for(INSTRUCTION, "instruction")
add_test_for(IMMEDIATE, "immediate")
add_test_for(MACHINE, "machine")
add_test_for(MEMORY, "memory")
add_test_for(SET, "set")
add_test_for(TYPE, "unsigned")
add_test_for(REGISTER, "register")

Suite *keyword_suite() {
  Suite *suite = suite_create("Keywords");
  TCase *tc_keywords = tcase_create("keywords");
  tcase_add_test(tc_keywords, test_INSTRUCTION);
  tcase_add_test(tc_keywords, test_IMMEDIATE);
  tcase_add_test(tc_keywords, test_MACHINE);
  tcase_add_test(tc_keywords, test_MEMORY);
  tcase_add_test(tc_keywords, test_SET);
  tcase_add_test(tc_keywords, test_TYPE);
  tcase_add_test(tc_keywords, test_REGISTER);
  suite_add_tcase(suite, tc_keywords);
  return suite;
}
