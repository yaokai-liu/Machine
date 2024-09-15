/**
 * Project Name: machine
 * Module Name: test/tokenize
 * Filename: tokenize.h
 * Creator: Yaokai Liu
 * Create Date: 2024-09-14
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TEST_TOKENIZE_H
#define MACHINE_TEST_TOKENIZE_H

#include <check.h>

Suite *symbol_suite();
Suite *keyword_suite();
Suite *number_suite();
Suite *identifier_suite();
Suite *united_suite();

#endif  // MACHINE_TEST_TOKENIZE_H
