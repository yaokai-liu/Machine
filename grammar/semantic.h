/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: semantic.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-08
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_SEMANTIC_H
#define MACHINE_SEMANTIC_H

#include "context.h"
#include "target.h"

int32_t check_mapping_item(GContext *context, BitField *bit_field, Evaluable *evaluable);

#endif  // MACHINE_SEMANTIC_H
