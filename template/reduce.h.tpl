/**
 * Project Name: machine
 * Module Name: template
 * Filename: reduces.gen.h
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_REDUCES_H
#define MACHINE_REDUCES_H

#include "parse.h"
#include "target.h"

enum __PRODUCT_RULE_ENUM__ {
  ${enum_reduces}
};

${reduces}

#endif  // MACHINE_REDUCES_H
