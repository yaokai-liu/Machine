${license}

#ifndef MACHINE_ACTION_TABLE_GEN_H
#define MACHINE_ACTION_TABLE_GEN_H

#include <stdint.h>

enum MACHINE_STATE_ENUM {
  ${state_enum}
};

const struct grammar_action *getParseAction(uint32_t _state, uint32_t ahead);

int32_t parseJumpState(uint32_t _state, uint32_t current);

uint32_t getParseStateCurrentTokenType(int32_t state);

uint32_t getParseStateExpectedTokenType(int32_t _state, uint32_t *token_types);

#endif  // MACHINE_ACTION_TABLE_GEN_H