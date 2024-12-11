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

int32_t gen_encoding_dec(
    Generator *generator, Identifier *names[], InstrForm *forms[], uint32_t n_forms
);
int32_t gen_encoding_def(
    Generator *generator, Identifier *names[], InstrForm *forms[], uint32_t n_forms
);

int32_t codegen_instr_form(GContext *context, InstrForm *form, Array *buffer);

#endif  // MACHINE_ENCODING_H
