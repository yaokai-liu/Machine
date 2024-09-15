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
#include "terminal.h"

typedef struct Entry {
    uint32_t type;
    void * target;
} Entry;
typedef Array Entries;  // Array<Entry>
typedef struct Machine {
    String * name;
    Entries * entries;
} Machine;
typedef struct Immediate {
    uint32_t width;
    uint32_t type;
    String * name;
} Immediate;
typedef Array PatternArgs;  // Array<IDENTIFIER>
typedef struct Pattern {
    PatternArgs * args;
} Pattern;
typedef struct Evaluable {
    uint32_t type;
    void *target;
} Evaluable;
typedef struct MappingItem {
    BitField * field;
    Evaluable * evaluable;
}MappingItem;
typedef Array MappingItems;  // Array<MappingItem>
typedef struct Layout {
    uint32_t type;
    void * target;
} Layout;
typedef struct InstrPart {
    uint32_t type;
    uint32_t width;
    Layout * layout;
} InstrPart;
typedef Array InstrParts;  // Array<InstrPart>
typedef struct InstrForm {
    uint32_t width;
    uint32_t tick;
    Pattern * pattern;
    InstrParts * parts;
} InstrForm;
typedef Array InstrForms;  // Array<InstrForm>
typedef struct Instruction {
    String * name;
    InstrForms * forms;
} Instruction;
typedef struct MemItem {
    uint32_t type;
    BitField * field;
} MemItem;
typedef struct Memory {
    String * name;
    uint32_t width;
    BitField *base;
    BitField *offset;
}Memory;
typedef struct Register {
    String * name;
    BitField * field;
    uint64_t code;
} Register;
typedef Array Registers;  // Array<Registers>
typedef struct RegisterGroup {
    String *name;
    uint32_t width;
    Registers * registers;
} RegisterGroup;
typedef Array SetItems;  // Array<SetItem>
typedef struct Set {
    String *name;
    SetItems * items;
} Set;
#include "allocator.h"

typedef void *fn_product(void *argv[], const Allocator *allocator);

extern fn_product * const PRODUCTS[];

#endif  // MACHINE_TARGET_H
