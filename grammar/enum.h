/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: enum.h
 * Creator: Yaokai Liu
 * Create Date: 2024-09-09
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ENUM_H
#define MACHINE_ENUM_H

enum IMMEDIATE_TYPE {
  IT_UNSIGNED,
  IT_SIGNED,
};

enum BOOL_COND_OP {
  CB_IN = 128,
  CB_LT,
  CB_LE,
  CB_GT,
  CB_GE,
  CB_EQ,
  CB_NE,
  CB_BIT_OR,
  CB_BIT_AND,
  CB_BIT_XOR,
  CS_BIT_INV,

  BOOL_AND_ARRAY,
  BOOL_OR_ARRAY,
};

#endif  // MACHINE_ENUM_H
