${license}

#ifndef MACHINE_ACTION_TABLE_GEN_H
#define MACHINE_ACTION_TABLE_GEN_H

#include <stdint.h>

enum __MACHINE_STATE_ENUM__ {
  ${state_enum}
};

const struct grammar_action *getParseAction(uint32_t index, uint32_t ahead);

int32_t parseJumpState(uint32_t index, uint32_t current);

uint32_t getParseStateCurrentTokenType(int32_t state);

#endif  // MACHINE_ACTION_TABLE_GEN_H