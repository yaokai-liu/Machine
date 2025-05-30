${license}

#ifndef MACHINE_TOKENS_H
#define MACHINE_TOKENS_H

#include "char_t.h"
#include <stdint.h>

enum TOKEN_TYPE_ENUM: uint32_t {
  ${enums},
  Machine_TOKEN_PLACE_HOLDER,
  MAX_TOTAL_TOKEN,
  Machine_TOKEN_BAD_TOKEN = UINT32_MAX
};
extern const char_t *TOKEN_NAMES[];
#endif  // MACHINE_TOKENS_H
