/**
 * Project Name: machine
 * Module Name: contana
 * Filename: string_t.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "string_t.h"
#include <stdint.h>

inline uint32_t strcmp_o(const char_t * const str1, const char_t * const str2) {
  int len = 0;
  while (str1[len] && str1[len] == str2[len]) { len++; }
  return len;
}

inline uint32_t stridx_o(const char_t chr, const char_t * const str) {
  int len = 0;
  while (chr != str[len] && str[len]) { len++; }
  return len;
}
