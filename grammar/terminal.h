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

typedef struct Terminal {
  uint32_t type;
  uint32_t length;
  uint32_t lineno;
  uint32_t column;
  void *value;
} Terminal;

typedef struct Identifier {
  char_t *ptr;
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
