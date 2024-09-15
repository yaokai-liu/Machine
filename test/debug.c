/**
 * Project Name: machine
 * Module Name: test
 * Filename: debug.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-14
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "allocator.h"
#include "terminal.h"
#include "tokenize.h"
#include <stdio.h>

int main() {
  const char_t *testString = "[4-7]";
  uint32_t cost = 0, n_tokens = 0;
  const Terminal *terminals = tokenize(testString, &cost, &n_tokens, &STDAllocator);
  for (uint32_t i = 0; i < n_tokens; i++) { printf("%s\n", get_name(terminals[i].type)); }

  return 0;
}
