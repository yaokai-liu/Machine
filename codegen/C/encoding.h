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
#include "codegen.h"
#include "target.h"
#include "terminal.h"
#include <stdint.h>

int32_t online_gen_instr_encoding_dec(
    GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);
int32_t online_gen_instr_encoding_def(
    GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);
int32_t online_gen_instr_encoding_op(
    GContext *context, Array *buffer, char_t *name, const InstrForm *forms, uint32_t n_forms
);
int32_t gen_instr_encoding_mat(
    GContext *context, const Machine *machine, Array *key_buffer, Array *state_buffer
);

int32_t codegen_instr_form(GContext *context, Array *buffer, const InstrForm *form);

int32_t codegen_layout(GContext *context, Array *buffer, const Layout *layout, uint32_t width);

int32_t codegen_items_bf(
    GContext *context, Array *buffer, MappingItems *items, const BitField *bit_field
);

int32_t eval_to_val(GContext *context, Evaluable *evaluable, char_t *buffer);

#endif  // MACHINE_ENCODING_H
