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
#include "common/check-string.h"
#include "terminal.h"
#include "tokenize.h"
#include "tokens.gen.h"
#include <check.h>
#include <stdint.h>

#define lenof(str_literal) ((sizeof str_literal) - 1)

START_TEST(test_IDENTIFIER_lower) {
  const char_t *string = "abcdefg hijklmn opqrst uvwxyz";
  Identifier *identifier;
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("abcdefg hijklmn opqrst uvwxyz"));
  ck_assert_uint_eq(n_tokens, 5);
  ck_assert_ptr_ne(terminals, nullptr);

  ck_assert_uint_eq(terminals[0].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[0].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[0].value, nullptr);
  ck_assert_uint_eq(terminals[0].lineno, 0);
  ck_assert_uint_eq(terminals[0].column, 0);
  ck_assert_uint_eq(terminals[0].length, lenof("abcdefg"));
  identifier = (Identifier *) terminals[0].value;
  ck_assert_uint_eq(identifier->len, lenof("abcdefg"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "abcdefg", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[1].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[1].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[1].value, nullptr);
  ck_assert_uint_eq(terminals[1].lineno, 0);
  ck_assert_uint_eq(terminals[1].column, lenof("abcdefg "));
  ck_assert_uint_eq(terminals[1].length, lenof("hijklmn"));
  identifier = (Identifier *) terminals[1].value;
  ck_assert_uint_eq(identifier->len, lenof("hijklmn"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "hijklmn", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[2].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[2].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[2].value, nullptr);
  ck_assert_uint_eq(terminals[2].lineno, 0);
  ck_assert_uint_eq(terminals[2].column, lenof("abcdefg hijklmn "));
  ck_assert_uint_eq(terminals[2].length, lenof("opqrst"));
  identifier = (Identifier *) terminals[2].value;
  ck_assert_uint_eq(identifier->len, lenof("opqrst"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "opqrst", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[3].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[3].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[3].value, nullptr);
  ck_assert_uint_eq(terminals[3].lineno, 0);
  ck_assert_uint_eq(terminals[3].column, lenof("abcdefg hijklmn opqrst "));
  ck_assert_uint_eq(terminals[3].length, lenof("uvwxyz"));
  identifier = (Identifier *) terminals[3].value;
  ck_assert_uint_eq(identifier->len, lenof("uvwxyz"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "uvwxyz", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[4].type, enum_TERMINATOR);
  ck_assert_str_eq(get_name(terminals[4].type), "TERMINATOR");
  ck_assert_ptr_eq(terminals[4].value, nullptr);
  ck_assert_uint_eq(terminals[4].lineno, 0);
  ck_assert_uint_eq(terminals[4].column, lenof("abcdefg hijklmn opqrst uvwxyz"));
  ck_assert_uint_eq(terminals[4].length, 0);
  STDAllocator.free((void *) terminals);
  ck_assert_uint_eq(lineno, 0);
  ck_assert_uint_eq(column, lenof("abcdefg hijklmn opqrst uvwxyz"));
}
END_TEST

START_TEST(test_IDENTIFIER_upper) {
  const char_t *string = "ABCDEFG HIJKLMN OPQRST UVWXYZ";
  Identifier *identifier;
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("ABCDEFG HIJKLMN OPQRST UVWXYZ"));
  ck_assert_uint_eq(n_tokens, 5);
  ck_assert_ptr_ne(terminals, nullptr);

  ck_assert_uint_eq(terminals[0].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[0].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[0].value, nullptr);
  identifier = (Identifier *) terminals[0].value;
  ck_assert_uint_eq(identifier->len, lenof("ABCDEFG"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "ABCDEFG", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[1].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[1].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[1].value, nullptr);
  identifier = (Identifier *) terminals[1].value;
  ck_assert_uint_eq(identifier->len, lenof("HIJKLMN"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "HIJKLMN", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[2].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[2].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[2].value, nullptr);
  identifier = (Identifier *) terminals[2].value;
  ck_assert_uint_eq(identifier->len, lenof("OPQRST"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "OPQRST", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[3].type, enum_IDENTIFIER);
  ck_assert_str_eq(get_name(terminals[3].type), "IDENTIFIER");
  ck_assert_ptr_ne(terminals[3].value, nullptr);
  identifier = (Identifier *) terminals[3].value;
  ck_assert_uint_eq(identifier->len, lenof("UVWXYZ"));
  ck_assert_uint_eq(strcmp_i(identifier->ptr, "UVWXYZ", identifier->len), identifier->len);

  ck_assert_uint_eq(terminals[4].type, enum_TERMINATOR);
  ck_assert_str_eq(get_name(terminals[4].type), "TERMINATOR");
  ck_assert_ptr_eq(terminals[4].value, nullptr);
  STDAllocator.free((void *) terminals);
}

END_TEST

Suite *identifier_suite() {
  Suite *suite = suite_create("Identifiers");
  TCase *tc_identifiers = tcase_create("identifiers");
  tcase_add_test(tc_identifiers, test_IDENTIFIER_lower);
  tcase_add_test(tc_identifiers, test_IDENTIFIER_upper);
  suite_add_tcase(suite, tc_identifiers);
  return suite;
}
