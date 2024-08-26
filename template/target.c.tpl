/**
 * Project Name: regex
 * Module Name: grammar/templates
 * Filename: target.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "target.h"
#include "reduce.gen.h"

fn_product * const PRODUCTS[] = {
  ${assign_reduces}
};
