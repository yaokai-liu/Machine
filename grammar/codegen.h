/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: generator.h
 * Creator: Yaokai Liu
 * Create Date: 2024-10-20
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/


#ifndef MACHINE_CODEGEN_H
#define MACHINE_CODEGEN_H

#include "array.h"
#include <stdint.h>

typedef int32_t codegen_t(Array * buffer, void * object);

#endif //MACHINE_CODEGEN_H
