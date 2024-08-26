/**
 * Project Name: regex
 * Module Name:
 * Filename: action-table.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "action-table.h"
#include "reduce.gen.h"
#include "tokens.gen.h"

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

const struct grammar_action ACTIONS[];
const uint16_t JUMPS[];
const struct unit UNITS[];

const struct unit *getUnit(const state *state, uint32_t look);

enum __STATE_ENUM__ {
    ${state_enum}
};

const struct grammar_action ACTIONS[] = {
  ${actions}
};

const uint16_t JUMPS[] = {
  ${jumps}
};

const struct unit UNITS[] = {
  ${units}
};

const struct state STATES[] = {
  ${states}
};


inline const struct unit *getUnit(const state *state, uint32_t look) {
    const struct unit *unit, *base = &UNITS[state->token_base];
    uint32_t left = 0, right = state->n_tokens - 1;
    uint32_t u_idx = (left + right) / 2;
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


inline const struct grammar_action *getAction(uint32_t index, uint32_t ahead) {
    const state *state = &STATES[index];
    const struct unit *unit = getUnit(state, ahead);
    if (!unit) { return nullptr; }
    const struct grammar_action *act = &ACTIONS[state->ndx_base + unit->offset];
    return act;
}

inline int32_t jump(uint32_t index, uint32_t current) {
    const state *state = &STATES[index];
    const struct unit *unit = getUnit(state, current);
    if (!unit) { return -1; }
    return JUMPS[state->goto_base + unit->offset];
}
