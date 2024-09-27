/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: test-identifier.c
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

uint32_t strcmp_i(const char_t *str1, const char_t *str2, uint32_t max_len) {
  uint32_t ndx = 0;
  while (str1[ndx] == str2[ndx] && ndx < max_len) { ndx++; }
  return ndx;
}

START_TEST(test_IDENTIFIER_lower) {
  const char_t *string = "abcdefg hijklmn opqrst uvwxyz";
  uint32_t cost, n_tokens;
  String *identifier;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("abcdefg hijklmn opqrst uvwxyz"));
  ck_assert_uint_eq(n_tokens, 5);
  ck_assert_ptr_ne(terminals, nullptr);

  ck_assert_uint_eq(terminals[0].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[0].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[0].value, nullptr);
  identifier = (String *) terminals[0].value;
  ck_assert_uint_eq(identifier->len, lenof("abcdefg"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "abcdefg", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[1].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[1].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[1].value, nullptr);
  identifier = (String *) terminals[1].value;
  ck_assert_uint_eq(identifier->len, lenof("hijklmn"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "hijklmn", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[2].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[2].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[2].value, nullptr);
  identifier = (String *) terminals[2].value;
  ck_assert_uint_eq(identifier->len, lenof("opqrst"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "opqrst", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[3].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[3].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[3].value, nullptr);
  identifier = (String *) terminals[3].value;
  ck_assert_uint_eq(identifier->len, lenof("uvwxyz"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "uvwxyz", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[4].type, enum_TERMINATOR);
  ck_assert_str_eq(get_name(terminals[4].type), "TERMINATOR");
  ck_assert_ptr_eq(terminals[4].value, nullptr);
  STDAllocator.free((void *) terminals);
}
END_TEST

START_TEST(test_IDENTIFIER_upper) {
  const char_t *string = "ABCDEFG HIJKLMN OPQRST UVWXYZ";
  uint32_t cost, n_tokens;
  String *identifier;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("ABCDEFG HIJKLMN OPQRST UVWXYZ"));
  ck_assert_uint_eq(n_tokens, 5);
  ck_assert_ptr_ne(terminals, nullptr);

  ck_assert_uint_eq(terminals[0].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[0].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[0].value, nullptr);
  identifier = (String *) terminals[0].value;
  ck_assert_uint_eq(identifier->len, lenof("ABCDEFG"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "ABCDEFG", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[1].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[1].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[1].value, nullptr);
  identifier = (String *) terminals[1].value;
  ck_assert_uint_eq(identifier->len, lenof("HIJKLMN"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "HIJKLMN", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[2].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[2].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[2].value, nullptr);
  identifier = (String *) terminals[2].value;
  ck_assert_uint_eq(identifier->len, lenof("OPQRST"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "OPQRST", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[3].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[3].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[3].value, nullptr);
  identifier = (String *) terminals[3].value;
  ck_assert_uint_eq(identifier->len, lenof("UVWXYZ"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "UVWXYZ", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[4].type, enum_TERMINATOR);
  ck_assert_str_eq(get_name(terminals[4].type), "TERMINATOR");
  ck_assert_ptr_eq(terminals[4].value, nullptr);
  STDAllocator.free((void *) terminals);
}

END_TEST

Suite * identifier_suite() {
  Suite *suite = suite_create("Identifiers");
  TCase *tc_identifiers = tcase_create("identifiers");
  tcase_add_test(tc_identifiers, test_IDENTIFIER_lower);
  tcase_add_test(tc_identifiers, test_IDENTIFIER_upper);
  suite_add_tcase(suite, tc_identifiers);
  return suite;
}