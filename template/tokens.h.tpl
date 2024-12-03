/**
 * Project Name: machine
 * Module Name: template
 * Filename: tokens.gen.h
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TOKENS_H
#define MACHINE_TOKENS_H

#include "char_t.h"

enum __MACHINE_TOKEN_TYPE_ENUM__ {
  ${enums}
};
extern const char_t *MACHINE_TOKEN_NAMES[];
#endif  // MACHINE_TOKENS_H
