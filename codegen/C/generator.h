/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generator.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GENERATOR_H
#define MACHINE_GENERATOR_H

#include "array.h"
#include "char_t.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Generator {
  const Allocator *allocator;
  const char_t *outname;
  const char_t *headpath;
  const char_t *libpath;
  const char_t *cr_holder;
  const char_t *year;
  Array /*<char_t>*/ *buffers[16];
} Generator;

#endif  // MACHINE_GENERATOR_H
