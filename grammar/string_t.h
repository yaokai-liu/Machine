/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: string_t.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-05
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_STRING_T_H
#define MACHINE_STRING_T_H

#include "array.h"
#include "char_t.h"
#include <stdint.h>

uint32_t strcmp_o(const char_t *str1, const char_t *str2);

uint32_t stridx_o(const char_t chr, const char_t *str);

uint32_t strlen_o(const char_t * const str);

#endif  // MACHINE_STRING_T_H
