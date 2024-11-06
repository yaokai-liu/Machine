/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: test-unities.c
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

START_TEST(test_BIT_FIELD) {
  const char_t *string = "[23-12]";
  uint32_t cost, n_tokens;
  const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
  ck_assert_uint_eq(cost, lenof("[23-12]"));
  ck_assert_uint_eq(n_tokens, 2);
  ck_assert_ptr_ne(terminals, nullptr);

  ck_assert_uint_eq(terminals[0].type, enum_BIT_FIELD);
  ck_assert_str_eq(get_name(terminals[0].type), "BIT_FIELD");
  ck_assert_ptr_ne(terminals[0].value, nullptr);
  BitField * bit_field = (BitField *) terminals[0].value;
  ck_assert_uint_eq(bit_field->lower, 12);
  ck_assert_uint_eq(bit_field->upper, 23);

  ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
  ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
  ck_assert_ptr_eq(terminals[1].value, nullptr);
  STDAllocator.free(bit_field);
  STDAllocator.free((void *) terminals);
}
END_TEST

START_TEST(test_BIT_FIELD_reverse) {
    const char_t *string = "[12-23]";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("[12-23]"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_BIT_FIELD);
    ck_assert_str_eq(get_name(terminals[0].type), "BIT_FIELD");
    ck_assert_ptr_ne(terminals[0].value, nullptr);
    BitField *bit_field = (BitField *) terminals[0].value;
    ck_assert_uint_eq(bit_field->lower, 12);
    ck_assert_uint_eq(bit_field->upper, 23);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free(bit_field);
    STDAllocator.free((void *) terminals);
}

END_TEST


START_TEST(test_BIT_FIELD_other) {
    const char_t *string = "[...]";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("[...]"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_BIT_FIELD);
    ck_assert_str_eq(get_name(terminals[0].type), "BIT_FIELD");
    ck_assert_ptr_eq(terminals[0].value, nullptr);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free((void *) terminals);
}
END_TEST


START_TEST(test_WIDTH_bit) {
    const char_t *string = "[23-bit]";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("[23-bit]"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_WIDTH);
    ck_assert_str_eq(get_name(terminals[0].type), "WIDTH");
    ck_assert_uint_eq((uint64_t) terminals[0].value, 23);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free((void *) terminals);
}
END_TEST

START_TEST(test_WIDTH_default_bit) {
    const char_t *string = "[23]";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("[23]"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_WIDTH);
    ck_assert_str_eq(get_name(terminals[0].type), "WIDTH");
    ck_assert_uint_eq((uint64_t) terminals[0].value, 23);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free((void *) terminals);
}
END_TEST

START_TEST(test_WIDTH_byte) {
    const char_t *string = "[46-byte]";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("[46-byte]"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_WIDTH);
    ck_assert_str_eq(get_name(terminals[0].type), "WIDTH");
    ck_assert_uint_eq((uint64_t) terminals[0].value, 46 * 8);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free((void *) terminals);
}
END_TEST

START_TEST(test_TIME_TICK) {
    const char_t *string = "(4-tick)";
    uint32_t cost, n_tokens;
    const Terminal *terminals = tokenize(string, &cost, &n_tokens, &STDAllocator);
    ck_assert_uint_eq(cost, lenof("(4-tick)"));
    ck_assert_uint_eq(n_tokens, 2);
    ck_assert_ptr_ne(terminals, nullptr);

    ck_assert_uint_eq(terminals[0].type, enum_TIME_TICK);
    ck_assert_str_eq(get_name(terminals[0].type), "TIME_TICK");
    ck_assert_uint_eq((uint64_t) terminals[0].value, 4);

    ck_assert_uint_eq(terminals[1].type, enum_TERMINATOR);
    ck_assert_str_eq(get_name(terminals[1].type), "TERMINATOR");
    ck_assert_ptr_eq(terminals[1].value, nullptr);
    STDAllocator.free((void *) terminals);
}
END_TEST

Suite * united_suite() {
  Suite *suite = suite_create("United");
  TCase *tc_united = tcase_create("united");
  tcase_add_test(tc_united, test_BIT_FIELD);
  tcase_add_test(tc_united, test_BIT_FIELD_reverse);
  tcase_add_test(tc_united, test_BIT_FIELD_other);
  tcase_add_test(tc_united, test_WIDTH_bit);
  tcase_add_test(tc_united, test_WIDTH_default_bit);
  tcase_add_test(tc_united, test_WIDTH_byte);
  tcase_add_test(tc_united, test_TIME_TICK);
  suite_add_tcase(suite, tc_united);
  return suite;
}
