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

enum MEM_KEY {
  MEM_BASE = 1,
  MEM_OFFSET,
};

enum PART_KEY {
  PART_PREFIX = 1,
  PART_SUFFIX,
  PART_PRINCIPAL
};

enum WIDTH_TYPE {
  WIDTH_BIT,
  WIDTH_BYTE
};

#endif  // MACHINE_ENUM_H
