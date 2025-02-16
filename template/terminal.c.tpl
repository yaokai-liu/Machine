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
 * Filename: terminals.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/
#include "terminal.h"
#include "tokens.gen.h"
const char_t TERMINALS[] = {
  ${terminals}
};

const char_t * TERMINAL_STRINGS[] = {
  ${strings}
};

const uint32_t TERMINAL_STRING_LENS[] = {
  ${string_lens}
};

const int32_t N_TERMINAL = sizeof(TERMINALS);