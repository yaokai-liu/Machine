/**
 * Project Name: machine
 * Module Name:
 * Filename: action-table.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ACTION_TABLE_H
#define MACHINE_ACTION_TABLE_H

#include <stdint.h>

typedef struct state state;
struct grammar_action {
  enum : uint8_t {
    reject = 0,
    stack = 1,
    reduce = 2
  } action      : 4;
  uint8_t count : 4;
  uint8_t type;
  const uint16_t offset;
};

const struct grammar_action *getAction(uint32_t index, uint32_t ahead);

int32_t jump(uint32_t index, uint32_t current);

uint32_t stateCurrentTokenType(int32_t state);

#endif  // MACHINE_ACTION_TABLE_H
