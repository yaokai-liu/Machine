/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Project Name: machine
 * Module Name: template
 * Filename: action-table.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "action.h"
#include "generated/tokens.gen.h"
#include "generated/macro/reduce.gen.h"
#include "generated/macro/action-table.gen.h"

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

const struct grammar_action MACRO_ACTIONS[];
const uint16_t MACRO_JUMPS[];
const struct unit MACRO_UNITS[];
const struct state MACRO_STATES[];
const uint32_t MACRO_CURRENT_TOKENS[];

const struct unit *getMacroParseUnit(const state *state, uint32_t look);

const struct grammar_action MACRO_ACTIONS[] = {
  ${actions}
};

const uint16_t MACRO_JUMPS[] = {
  ${jumps}
};

const struct unit MACRO_UNITS[] = {
  ${units}
};

const struct state MACRO_STATES[] = {
  ${states}
};

const uint32_t MACRO_CURRENT_TOKENS[] = {
  ${currents}
};

inline const struct unit *getMacroParseUnit(const state *state, uint32_t look) {
    const struct unit *unit, *base = &MACRO_UNITS[state->token_base];
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


inline const struct grammar_action *getMacroParseAction(uint32_t index, uint32_t ahead) {
    const state *state = &MACRO_STATES[index];
    const struct unit *unit = getMacroParseUnit(state, ahead);
    if (!unit) { return nullptr; }
    const struct grammar_action *act = &MACRO_ACTIONS[state->ndx_base + unit->offset];
    return act;
}

inline int32_t macroParseJumpState(uint32_t index, uint32_t current) {
    const state *state = &MACRO_STATES[index];
    const struct unit *unit = getMacroParseUnit(state, current);
    if (!unit) { return -1; }
    return MACRO_JUMPS[state->goto_base + unit->offset];
}

inline uint32_t getMacroParseStateCurrentTokenType(int32_t state) {
  return MACRO_CURRENT_TOKENS[state];
}