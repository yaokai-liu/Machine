/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: single-token.c
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

#define add_test_for(_name, _value)                                                \
  START_TEST(test_##_name) {                                                       \
    char_t *string = _value;                                                       \
    uint32_t cost, n_tokens;                                                       \
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator); \
    ck_assert_uint_eq(cost, 1);                                                    \
    ck_assert_uint_eq(n_tokens, 2);                                                \
    ck_assert_ptr_ne(terminals, nullptr);                                          \
    ck_assert_uint_eq(terminals[0].type, enum_##_name);                            \
    ck_assert_ptr_eq(terminals[0].value, 0);                                       \
    ck_assert_str_eq(get_name(terminals[0].type), string_t(#_name));               \
    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);                         \
    ck_assert_ptr_eq(terminals[1].value, 0);                                       \
    ck_assert_str_eq(get_name(terminals[1].type), string_t("TERMINATOR"));         \
    STDAllocator.free((void *) terminals);                                         \
  }                                                                                \
  END_TEST

add_test_for(COMMA, ",")
add_test_for(DOT, ".")
add_test_for(COLON, ":")
add_test_for(SEMICOLON, ";")
add_test_for(LEFT_BRACKET, "{")
add_test_for(RIGHT_BRACKET, "}")
add_test_for(LEFT_SQUARE_BRACKET, "[")
add_test_for(RIGHT_SQUARE_BRACKET, "]")
add_test_for(EQUAL, "=")

Suite *symbol_suite() {
  Suite *suite = suite_create("Symbols");
  TCase *tc_symbols = tcase_create("symbols");
  tcase_add_test(tc_symbols, test_COMMA);
  tcase_add_test(tc_symbols, test_DOT);
  tcase_add_test(tc_symbols, test_COLON);
  tcase_add_test(tc_symbols, test_SEMICOLON);
  tcase_add_test(tc_symbols, test_LEFT_BRACKET);
  tcase_add_test(tc_symbols, test_RIGHT_BRACKET);
  tcase_add_test(tc_symbols, test_LEFT_SQUARE_BRACKET);
  tcase_add_test(tc_symbols, test_RIGHT_SQUARE_BRACKET);
  tcase_add_test(tc_symbols, test_EQUAL);
  suite_add_tcase(suite, tc_symbols);
  return suite;
}
