/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: char_t.h
 * Creator: Yaokai Liu
 * Create Date: 24-7-4
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef LIU_CHAR_T_H
#define LIU_CHAR_T_H

// Promise sizeof(char_t) < sizeof(uint64_t).
typedef char char_t;

#define char_t(_chr)   _chr
#define string_t(_str) _str

#endif  // LIU_CHAR_T_H
