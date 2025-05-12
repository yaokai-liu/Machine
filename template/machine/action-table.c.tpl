${license}

#include "action.h"
#include "generated/tokens.gen.h"
#include "generated/machine/reduce.gen.h"
#include "generated/machine/action-table.gen.h"

struct state {
  const uint16_t ndx_base;
  const uint16_t goto_base;
  const uint16_t token_base;
  const uint16_t n_tokens;
};
struct unit {
  uint8_t type;
  uint8_t offset;
};

const struct grammar_action MACHINE_ACTIONS[];
const uint16_t MACHINE_JUMPS[];
const struct unit MACHINE_UNITS[];
const struct state MACHINE_STATES[];
const uint32_t MACHINE_CURRENT_TOKENS[];

const struct unit *getParseUnit(const state *state, uint32_t look);

const struct grammar_action MACHINE_ACTIONS[] = {
  ${actions}
};

const uint16_t MACHINE_JUMPS[] = {
  ${jumps}
};

const struct unit MACHINE_UNITS[] = {
  ${units}
};

const struct state MACHINE_STATES[] = {
  ${states}
};

const uint32_t MACHINE_CURRENT_TOKENS[] = {
  ${currents}
};

inline const struct unit *getParseUnit(const state *state, uint32_t look) {
  const struct unit *unit, *base = &MACHINE_UNITS[state->token_base];
  int32_t left = 0, right = state->n_tokens - 1;
  int32_t u_idx = (left + right) / 2;
  unit = &base[u_idx];
  while (unit->type != look && left < right) {
    if (unit->type < look) {
      left = u_idx + 1;
    } else {
      right = u_idx - 1;
    }
    u_idx = (left + right) / 2;
    unit = &base[u_idx];
  }
  if (unit->type != look) { return nullptr; }
  return unit;
}

inline const struct grammar_action *getParseAction(uint32_t _state, uint32_t ahead) {
    const state *state = &MACHINE_STATES[_state];
    const struct unit *unit = getParseUnit(state, ahead);
    if (!unit) { return nullptr; }
    const struct grammar_action *act = &MACHINE_ACTIONS[state->ndx_base + unit->offset];
    return act;
}

inline int32_t parseJumpState(uint32_t _state, uint32_t current) {
    const state *state = &MACHINE_STATES[_state];
    const struct unit *unit = getParseUnit(state, current);
    if (!unit) { return -1; }
    return MACHINE_JUMPS[state->goto_base + unit->offset];
}

inline uint32_t getParseStateCurrentTokenType(int32_t state) {
  return MACHINE_CURRENT_TOKENS[state];
}

inline uint32_t getParseStateExpectedTokenType(int32_t _state, uint32_t *token_types) {
  const state *state = &MACHINE_STATES[_state];
  const struct unit *base = &MACHINE_UNITS[state->token_base];
  if (token_types) {
    for (uint32_t i = 0; i < state->n_tokens; i++) {
      token_types[i] = base[i].type;
    }
  }
  return state->n_tokens;
}
