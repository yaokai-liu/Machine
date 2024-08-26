/**
 * Project Name: regex
 * Module Name: template
 * Filename: terminals.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/
#include "terminal.h"
#include "tokens.gen.h"
const char_t TERMINALS[] = {
  ${terminals}
};

const char_t * const TERMINAL_STRINGS[] = {
  ${strings}
};
const int32_t N_TERMINAL = sizeof(TERMINALS);