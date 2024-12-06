/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: parse.h
 * Creator: Yaokai Liu
 * Create Date: 2024-10-27
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_PARSE_H
#define MACHINE_PARSE_H

#include "context.h"
#include "target.h"

typedef void *fn_reduce(void *argv[], GContext *context, const Allocator *allocator);

extern fn_reduce * const PRODUCTS[];

Machine *parse(const Terminal *tokens, uint32_t *cost, const Allocator *allocator);

#endif  // MACHINE_PARSE_H
