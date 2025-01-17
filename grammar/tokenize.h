/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: tokenize.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-31
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENIZE_H
#define MACHINE_TOKENIZE_H

#include "char_t.h"
#include "terminal.h"
#include <stdint.h>

const Terminal *tokenize(
    const char_t *input, uint32_t *cost, uint32_t *n_tokens, uint32_t *lineno, uint32_t *column,
    const Allocator *allocator
);

const char_t *get_name(uint16_t type);

#endif  // MACHINE_TOKENIZE_H
