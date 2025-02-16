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
 * Module Name: codegen/C
 * Filename: source.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-10
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ENCODING_H
#define MACHINE_ENCODING_H

#include "array.h"
#include "char_t.h"
#include "generate.h"
#include "target.h"
#include "terminal.h"
#include <stdint.h>

int32_t online_gen_instr_encoding_dec(
    const GContext *, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);
int32_t online_gen_instr_encoding_def(
    const GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);
void gen_instr_exec(Generator *generator, const Machine *machine);

void gen_jump_table_dec(Generator *generator, const Machine *machine);
void gen_jump_table_def(
    Generator *generator, const Machine *machine, Array *key_buffer, Array *state_buffer
);

int32_t codegen_instr_form(const GContext *context, Array *buffer, const InstrForm *form);

int32_t codegen_layout(
    const GContext *context, Array *buffer, const Layout *layout, uint32_t width,
    const Pattern *pattern, char_t *temp_buffer
);

int32_t codegen_items_bf(
    const GContext *context, Array *buffer, MappingItems *items, const BitField *bit_field,
    const Pattern *pattern, char_t *temp_buffer
);

int32_t eval_to_val(
    const GContext *context, Evaluable *evaluable, char_t *buffer, const Pattern *pattern
);

#endif  // MACHINE_ENCODING_H
