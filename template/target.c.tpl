/**
 * Project Name: machine
 * Module Name: template
 * Filename: target.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "parse.h"
#include "reduce.gen.h"

fn_produce * const PRODUCTS[] = {
  ${assign_reduces}
};
