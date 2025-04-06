/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: generator.h
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GENERATOR_H
#define MACHINE_GENERATOR_H

#include "array.h"
#include "avl-tree.h"
#include "char_t.h"
#include "elf/compositor.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Generator {
  const Allocator *allocator;
  const Array *ident_array;
  const char_t *outname;
  const char_t *headpath;
  const char_t *libpath;
  const char_t *cr_holder;
  const char_t *year;
  uint32_t gen_type;
} Generator;

enum GEN_TYPE_ENUM {
  GT_C,
  GT_X86_64,
};

enum GenC_ByteBuffer {
  GenC_exports,

  GenC_includes,
  GenC_macros,
  GenC_enums,
  GenC_types,
  GenC_declares,
  GenC_definitions,
  GenC_tables
};

enum GenElf_ByteBuffer {
  GenElf_exports,
  GenElf_enums,

  GenElf_bss,
  GenElf_data,
  GenElf_rodata,
  GenElf_text,
  GenElf_comment,
  GenElf_rel,
  GenElf_rela_rodata,
  GenElf_rela_text,
  GenElf_symtab,
  GenElf_strtab
};

typedef struct Generator Generator;
typedef struct CGenerator CGenerator;
typedef struct Elf64Generator Elf64Generator;

CGenerator *Generator_new_C(const Array *ident_array, const Allocator *allocator);
Elf64Generator *Generator_new_elf64(const Array *ident_array, const Allocator *allocator);

void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *headpath, const char_t *libpath,
    const char_t *cr_holder, const char_t *year
);
void Generator_destroy(Generator *generator);

Array *CGenerator_getOutputBuffer(CGenerator *generator, uint32_t index);
AVLTree *Elf64Generator_getInstrFormTree(Elf64Generator *generator);
AVLTree *Elf64Generator_getEnumTree(Elf64Generator *generator);
Array *Elf64Generator_getEnumArray(Elf64Generator *generator);
Elf64Compositor *Elf64Generator_getCompositor(Elf64Generator *generator);
void Elf64Generator_set_cose(Elf64Generator *generator, uint32_t val);
void Elf64Generator_set_cote(Elf64Generator *generator, uint32_t val);
uint32_t Elf64Generator_get_sym_index(Elf64Generator *generator, uint64_t v_form_ndx);

#endif  // MACHINE_GENERATOR_H
