/**
 * Project Name: machine
 * Module Name: test
 * Filename: test-tokenize.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-14
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "tokenize/test-tokenize.h"
#include <check.h>

int main() {
  SRunner *srunner = srunner_create(nullptr);
  srunner_add_suite(srunner, symbol_suite());
  srunner_add_suite(srunner, keyword_suite());
  srunner_add_suite(srunner, number_suite());
  srunner_add_suite(srunner, identifier_suite());
  srunner_add_suite(srunner, united_suite());
  srunner_set_fork_status(srunner, CK_NOFORK);
  srunner_run_all(srunner, CK_NORMAL);
  int n_failed = srunner_ntests_failed(srunner);
  srunner_free(srunner);

  return n_failed ? -1 : 0;
}
