/**
 * Project Name: machine
 * Module Name:
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_TARGET_H
#define MACHINE_TARGET_H

#include "array.h"

typedef struct Machine Machine;
typedef Array Entries;  // Array<Entry>
typedef struct Entry Entry;
typedef struct Immediate Immediate;
typedef struct InstrForm InstrForm;
typedef Array InstrForms;  // Array<InstrForm>
typedef struct Instruction Instruction;
typedef struct InstrPart InstrPart;
typedef Array InstrParts;  // Array<InstrPart>
typedef struct Memory Memory;
typedef struct MemItem MemItem;
typedef struct Register Register;
typedef struct RegisterGroup RegisterGroup;
typedef Array Registers;  // Array<Registers>
typedef struct Set Set;
typedef Array SetItems;  // Array<SetItem>
typedef struct Layout Layout;
typedef struct MappingItem MappingItem;
typedef Array MappingItems;  // Array<MappingItem>

#include "allocator.h"

typedef void *fn_product(void *argv[], const Allocator *allocator);

extern fn_product * const PRODUCTS[];

#endif  // MACHINE_TARGET_H
