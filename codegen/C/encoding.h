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
#include "parse/target.h"
#include "terminal.h"
#include <stdint.h>

void GenC_gen_instr_dec(const Generator *generator, const Machine *machine);
void GenC_gen_instr_def(const Generator *generator, const Machine *machine);

void GenC_gen_jump_table_dec(const Generator *generator, const Machine *machine);
void GenC_gen_jump_table_def(const Generator *generator, const Machine *machine);

void GenC_gen_instr_encoding_dec(
    const ParseContext *context, const Array *ident_array, const char_t *instr_op,
    const InstrForm forms[], uint32_t n_forms, FILE *ostream
);
void GenC_gen_instr_encoding_def(
    const ParseContext *context, const Array *ident_array, const char_t *instr_op,
    const InstrForm forms[], uint32_t n_forms, FILE *ostream
);
int32_t GenC_gen_instr_form(
    const ParseContext *context, const Array *ident_array, const InstrForm *form, FILE *ostream
);
void GenC_gen_form_check(
    const ParseContext *context, const Array *ident_array, const InstrForm *form, FILE *ostream
);
int32_t GenC_gen_instr_part(
    const ParseContext *context, const Array *ident_array, const InstrForm *form,
    const InstrPart *part, FILE *ostream
);
void GenC_gen_layout(
    const ParseContext *context, const Array *ident_array, const Layout *layout, uint32_t width,
    const Pattern *pattern, FILE *ostream
);
void GenC_gen_switchable(
    const ParseContext *context, const Array *ident_array, const Switchable *switchable,
    BitField *p_bf, const Pattern *pattern, FILE *ostream
);
void GenC_gen_mapping_item(
    const ParseContext *context, const Array *ident_array, MappingItems *items,
    const BitField *bit_field, const Pattern *pattern, FILE *ostream
);

int32_t expr_to_val(
    const ParseContext *context, const Array *ident_array, const CondExpr *expr,
    const Pattern *pattern, FILE *ostream
);
int32_t eval_to_val(
    const ParseContext *context, const Array *ident_array, const Evaluable *evaluable,
    const Pattern *pattern, FILE *ostream
);
void type_to_val(
    const ParseContext *context, const Array *ident_array, const Identifier *ident,
    const Pattern *pattern, FILE *ostream
);

#endif  // MACHINE_ENCODING_H
