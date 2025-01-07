/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generate.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GENERATE_H
#define MACHINE_GENERATE_H

#include "array.h"
#include "context.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Generator {
  const Allocator *allocator;
  GContext *context;
  Array *buffer;
} Generator;

codegen_t *get_codegen(uint32_t type);

#endif  // MACHINE_GENERATE_H
