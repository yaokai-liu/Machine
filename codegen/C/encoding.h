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

#include "codegen.h"
#include "generate.h"
#include "target.h"

int32_t gen_instr_encoding_dec(
    GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);
int32_t gen_instr_encoding_def(
    GContext *context, Array *buffer, const char_t *instr_op, const InstrForm forms[],
    uint32_t n_forms
);

int32_t codegen_instr_form(GContext *context, Array *buffer, const InstrForm *form);

int32_t codegen_layout(GContext *context, Array *buffer, const Layout *layout, uint32_t width);

int32_t codegen_items_bf(GContext *context, Array *buffer, MappingItems *items, const BitField *bf);

int32_t eval_to_val(GContext *context, Evaluable *evaluable, char_t *buffer);

#endif  // MACHINE_ENCODING_H
