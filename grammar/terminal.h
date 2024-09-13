/**
 * Project Name: machine
 * Module Name:
 * Filename: terminal.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TERMINAL_H
#define MACHINE_TERMINAL_H

#include "allocator.h"
#include "char_t.h"
#include <stdint.h>

typedef struct {
  uint8_t type;
  void *value;
} Terminal;

Terminal *tokenize(const char_t *input, uint32_t *cost, uint32_t *n_tokens, const Allocator *allocator);

extern const char_t TERMINALS[];
extern const char_t *TERMINAL_STRINGS[];
extern const uint32_t TERMINAL_STRING_LENS[];
extern const int32_t N_TERMINAL;

#endif  // MACHINE_TERMINAL_H
