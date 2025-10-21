/* License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2025 Yaokai Liu
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
 *
 * Project Name: machine
 * Module Name: codegen
 * Filename: generator.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-20
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "generator.h"
#include <stdio.h>

typedef struct CGenerator {
  Generator __SUPPER__;
} CGenerator;

void Generator_init(
    Generator *generator, const char_t *headpath, const char_t *libpath,
    enum GEN_TYPE_ENUM gen_type, const Array *ident_array, const Allocator *allocator
);

void Generator_init(
    Generator *generator, const char_t *headpath, const char_t *libpath,
    enum GEN_TYPE_ENUM gen_type, const Array *ident_array, const Allocator *allocator
) {
  generator->ident_array = ident_array;
  generator->allocator = allocator;
  generator->gen_type = gen_type;
  generator->headpath = headpath;
  generator->libpath = libpath;
  generator->ostream_head = fopen(headpath, "w");
  generator->ostream_lib = fopen(libpath, "w");
}

void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *cr_holder, const char_t *year
) {
  generator->outname = outname;
  generator->cr_holder = cr_holder;
  generator->year = year;
}

void Generator_destroy(Generator *generator) {
  if (generator->ostream_head) { fclose(generator->ostream_head); generator->ostream_head = nullptr; }
  if (generator->ostream_lib) { fclose(generator->ostream_lib); generator->ostream_lib = nullptr; }
  generator->allocator->free(generator);
}

#ifndef MACHINE_GENERATE_FORM_ELF

CGenerator *Generator_new_C(
  const Array *ident_array, const char_t *headpath, const char_t *libpath,
  const Allocator *allocator
) {
  CGenerator *generator = allocator->calloc(1, sizeof(CGenerator));
  Generator_init(&generator->__SUPPER__, headpath, libpath, GT_C, ident_array, allocator);
  return generator;
}

void CGenerator_destroy(CGenerator *c_generator) {
  Generator *generator = &c_generator->__SUPPER__;
  return Generator_destroy(generator);
}

#else

typedef struct Elf64Generator {
  Generator __SUPPER__;
  Elf64Compositor *compositor;
  AVLTree /*<VInstrIndex, uint32_t>*/ *instr_form_tree;
  AVLTree /*<Identifier, uint32_t>*/ *enum_tree;
  Array /*<uint32_t>*/ *enum_array;
  uint32_t count_of_single_entries;
  uint32_t count_of_total_entries;
} Elf64Generator;

inline void Elf64Generator_setCompositor(Elf64Generator *generator, Elf64Compositor *compositor) {
  generator->compositor = compositor;
}

inline AVLTree *Elf64Generator_getInstrFormTree(const Elf64Generator *generator) {
  return generator->instr_form_tree;
}
inline AVLTree *Elf64Generator_getEnumTree(const Elf64Generator *generator) {
  return generator->enum_tree;
}
inline Array *Elf64Generator_getEnumArray(const Elf64Generator *generator) {
  return generator->enum_array;
}
inline Elf64Compositor *Elf64Generator_getCompositor(const Elf64Generator *generator) {
  return generator->compositor;
}

inline void Elf64Generator_set_cose(Elf64Generator *generator, const uint32_t val) {
  generator->count_of_single_entries = val;
}

inline void Elf64Generator_set_cote(Elf64Generator *generator, const uint32_t val) {
  generator->count_of_total_entries = val;
}

uint32_t Elf64Generator_get_sym_index(Elf64Generator *, uint64_t) {
  return 0;
}

Elf64Generator *Generator_new_elf64(const Array *ident_array, const Allocator *allocator) {
  Elf64Generator *generator = allocator->calloc(1, sizeof(Elf64Generator));
  Generator_init(&generator->__SUPPER__, GT_X86_64, ident_array, allocator);
  generator->enum_array = Array_new(sizeof(uint32_t), GenElf_enums, allocator);
  generator->instr_form_tree = AVLTree_new(allocator, nullptr);
  generator->enum_tree = AVLTree_new(allocator, nullptr);
  return generator;
}

#endif
