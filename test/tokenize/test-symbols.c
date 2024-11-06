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
#include "enum.h"
#include <check.h>
#include <stdint.h>


#define add_test_for(_name, _symbol, _value)                                       \
  START_TEST(test_##_name) {                                                       \
    char_t *string = _symbol;                                                      \
    uint32_t cost, n_tokens;                                                       \
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator); \
    ck_assert_uint_eq(cost, 1);                                                    \
    ck_assert_uint_eq(n_tokens, 2);                                                \
    ck_assert_ptr_ne(terminals, nullptr);                                          \
    ck_assert_uint_eq(terminals[0].type, enum_##_name);                            \
    ck_assert_ptr_eq(terminals[0].value, (void *) (uint64_t) (_value));            \
    ck_assert_str_eq(get_name(terminals[0].type), string_t(#_name));               \
    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);                         \
    ck_assert_ptr_eq(terminals[1].value, 0);                                       \
    ck_assert_str_eq(get_name(terminals[1].type), string_t("TERMINATOR"));         \
    STDAllocator.free((void *) terminals);                                         \
  }                                                                                \
  END_TEST

add_test_for(COMMA, ",", 0)
add_test_for(DOT, ".", 0)
add_test_for(COLON, ":", 0)
add_test_for(SEMICOLON, ";", 0)
add_test_for(LEFT_BRACKET, "{", 0)
add_test_for(RIGHT_BRACKET, "}", 0)
add_test_for(LEFT_SQUARE_BRACKET, "[", 0)
add_test_for(RIGHT_SQUARE_BRACKET, "]", 0)
add_test_for(EQUAL, "=", 0)

#define add_test_for_type(_test_name, _type, _symbol, _value)                      \
  START_TEST(test_##_test_name) {                                                  \
    char_t *string = _symbol;                                                      \
    uint32_t cost, n_tokens;                                                       \
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator); \
    ck_assert_uint_eq(cost, 1);                                                    \
    ck_assert_uint_eq(n_tokens, 2);                                                \
    ck_assert_ptr_ne(terminals, nullptr);                                          \
    ck_assert_uint_eq(terminals[0].type, enum_##_type);                            \
    ck_assert_ptr_eq(terminals[0].value, (void *) (uint64_t) (_value));            \
    ck_assert_str_eq(get_name(terminals[0].type), string_t(#_type));               \
    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);                         \
    ck_assert_ptr_eq(terminals[1].value, 0);                                       \
    ck_assert_str_eq(get_name(terminals[1].type), string_t("TERMINATOR"));         \
    STDAllocator.free((void *) terminals);                                         \
  }                                                                                \
  END_TEST

add_test_for_type(MEM_KEY_1,  MEM_KEY, "$", MEM_BASE)
add_test_for_type(MEM_KEy_2,  MEM_KEY, ">", MEM_OFFSET)
add_test_for_type(PART_KEY_1, PART_KEY, "^", PART_PREFIX)
add_test_for_type(PART_KEY_2, PART_KEY, "~", PART_PRINCIPAL)
add_test_for_type(PART_KEY_3, PART_KEY, "&", PART_SUFFIX)


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
  tcase_add_test(tc_symbols, test_MEM_KEY_1);
  tcase_add_test(tc_symbols, test_MEM_KEy_2);
  tcase_add_test(tc_symbols, test_PART_KEY_1);
  tcase_add_test(tc_symbols, test_PART_KEY_2);
  tcase_add_test(tc_symbols, test_PART_KEY_3);
  suite_add_tcase(suite, tc_symbols);
  return suite;
}
