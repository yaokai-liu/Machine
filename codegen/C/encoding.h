/**
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
void gen_instr_exec_and_encoding(Generator *generator, const Machine *machine);

void gen_jump_table_dec(Generator *generator, const Machine *machine);
void gen_jump_table_def(Generator *generator, const Machine *machine, Array *key_buffer, Array *state_buffer);

int32_t codegen_instr_form(const GContext *context, Array *buffer, const InstrForm *form);

int32_t codegen_layout(const GContext *context, Array *buffer, const Layout *layout, uint32_t width);

int32_t codegen_items_bf(
    const GContext *context, Array *buffer, MappingItems *items, const BitField *bit_field
);

int32_t eval_to_val(const GContext *context, Evaluable *evaluable, char_t *buffer);

#endif  // MACHINE_ENCODING_H
