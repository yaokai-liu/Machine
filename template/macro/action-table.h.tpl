${license}

#ifndef MACRO_ACTION_TABLE_GEN_H
#define MACRO_ACTION_TABLE_GEN_H

#include <stdint.h>

enum __MACRO_STATE_ENUM__ {
  ${state_enum}
};

const struct grammar_action *getMacroParseAction(uint32_t _state, uint32_t ahead);

int32_t macroParseJumpState(uint32_t _state, uint32_t current);

uint32_t getMacroParseStateCurrentTokenType(int32_t state);

uint32_t getParseStateExpectedTokenType(int32_t _state, uint32_t *token_types);

#endif  // MACRO_ACTION_TABLE_GEN_H