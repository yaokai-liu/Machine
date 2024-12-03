/**
 * Project Name: machine
 * Module Name: template
 * Filename: terminals.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/
#include "terminal.h"
#include "tokens.gen.h"
const char_t TERMINALS[] = {
  ${terminals}
};

const char_t * TERMINAL_STRINGS[] = {
  ${strings}
};

const uint32_t TERMINAL_STRING_LENS[] = {
  ${string_lens}
};

const int32_t N_TERMINAL = sizeof(TERMINALS);