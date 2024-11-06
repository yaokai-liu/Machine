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

#include "target.h"

Machine *parse(const Terminal *tokens, const Allocator *allocator);

#endif //MACHINE_PARSE_H
