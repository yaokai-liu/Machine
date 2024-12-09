
/**
 * Project Name: machine
 * Module Name: codegen/C/template
 * Filename: machine.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "machine.h"

#define pushInstrBytes(...) do { \
  uint8_t bytes[] = {__VA_ARGS__}; \
  Array_append(buffer, bytes, sizeof(bytes)); \
} while (false)

#define pushInstrNamed(arg, count) do { \
    uint8_t bytes[count] = {}; \
    for (uint32_t i = 0; i < (count); i++) { \
        bytes[i] = (arg) & 0xFF; \
        (arg) >>= 8; \
    } \
    Array_append(buffer, bytes, sizeof(bytes)); \
} while (false)

#define pushInstrBitField(arg, bl, bh, count) do { \
    uint8_t bytes[count] = {}; \
    uint64_t number = ((arg) >> bl) & ((1 << (bh -bl + 1)) - 1); \
    for (uint32_t i = 0; i < (count); i++) { \
        bytes[i] = number & 0xFF; \
        number >>= 8; \
    } \
    Array_append(buffer, bytes, sizeof(bytes)); \
} while (false)

