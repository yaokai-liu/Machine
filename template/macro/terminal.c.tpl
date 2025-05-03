${license}

#include "terminal.h"
#include "generated/tokens.gen.h"
const char_t MACRO_TERMINALS[] = {
  ${terminals}
};

const char_t * MACRO_TERMINAL_STRINGS[] = {
  ${strings}
};

const uint32_t MACRO_TERMINAL_STRING_LENS[] = {
  ${string_lens}
};

const int32_t N_MACRO_TERMINAL = sizeof(MACRO_TERMINALS);