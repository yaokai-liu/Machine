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
  uint16_t type;
  void *value;
} Terminal;

const Terminal *
    tokenize(const char_t *input, uint32_t *cost, uint32_t *n_tokens, const Allocator *allocator);

typedef struct Identifier {
  const char_t *ptr;
  uint32_t len;
} Identifier;

typedef struct BitField {
  uint32_t upper;
  uint32_t lower;
} BitField;

extern const char_t TERMINALS[];
extern const char_t *TERMINAL_STRINGS[];
extern const uint32_t TERMINAL_STRING_LENS[];
extern const int32_t N_TERMINAL;

void releaseIdentifier(Identifier *ident, const Allocator *allocator);
void releaseBitField(BitField *bf, const Allocator *allocator);

#endif  // MACHINE_TERMINAL_H
