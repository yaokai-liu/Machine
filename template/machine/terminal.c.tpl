${license}

#include "terminal.h"
#include "generated/tokens.gen.h"
const char_t MACHINE_TERMINALS[] = {
  ${terminals}
};

const char_t * MACHINE_TERMINAL_STRINGS[] = {
  ${strings}
};

const uint32_t MACHINE_TERMINAL_STRING_LENS[] = {
  ${string_lens}
};

const int32_t N_MACHINE_TERMINAL = sizeof(MACHINE_TERMINALS);