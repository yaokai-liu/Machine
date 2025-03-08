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
 * Filename: action-table.gen.h
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACRO_ACTION_TABLE_GEN_H
#define MACRO_ACTION_TABLE_GEN_H

#include <stdint.h>

enum __MACRO_STATE_ENUM__ {
  ${state_enum}
};

const struct grammar_action *getMacroParseAction(uint32_t index, uint32_t ahead);

int32_t macroParseJumpState(uint32_t index, uint32_t current);

uint32_t getMacroParseStateCurrentTokenType(int32_t state);

#endif  // MACRO_ACTION_TABLE_GEN_H