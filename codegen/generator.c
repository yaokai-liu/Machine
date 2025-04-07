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
#include "elf/compositor.h"

typedef struct CGenerator {
  Generator __SUPPER__;
  Array /*<char_t>*/ *buffers[16];
} CGenerator;

typedef struct Elf64Generator {
  Generator __SUPPER__;
  Elf64Compositor *compositor;
  AVLTree /*<VInstrIndex, uint32_t>*/ *instr_form_tree;
  AVLTree /*<Identifier, uint32_t>*/ *enum_tree;
  Array /*<uint32_t>*/ *enum_array;
  uint32_t count_of_single_entries;
  uint32_t count_of_total_entries;
} Elf64Generator;

void Generator_init(
    Generator *generator, enum GEN_TYPE_ENUM gen_type, const Array *ident_array,
    const Allocator *allocator
) {
  generator->ident_array = ident_array;
  generator->allocator = allocator;
  generator->gen_type = gen_type;
}

CGenerator *Generator_new_C(const Array *ident_array, const Allocator *allocator) {
  CGenerator *generator = allocator->calloc(1, sizeof(CGenerator));
  Generator_init(&generator->__SUPPER__, GT_C, ident_array, allocator);
  return generator;
}

Elf64Generator *Generator_new_elf64(const Array *ident_array, const Allocator *allocator) {
  Elf64Generator *generator = allocator->calloc(1, sizeof(Elf64Generator));
  Generator_init(&generator->__SUPPER__, GT_X86_64, ident_array, allocator);
  generator->enum_array = Array_new(sizeof(uint32_t), GenElf_enums, allocator);
  generator->instr_form_tree = AVLTree_new(allocator, nullptr);
  generator->enum_tree = AVLTree_new(allocator, nullptr);
  generator->compositor = Elf64Compositor_new_sysv_2lsb(ET_REL, EM_X86_64, allocator);
  return generator;
}

void Generator_setCopyright(
    Generator *generator, const char_t *outname, const char_t *headpath, const char_t *libpath,
    const char_t *cr_holder, const char_t *year
) {
  generator->outname = outname;
  generator->headpath = headpath;
  generator->libpath = libpath;
  generator->cr_holder = cr_holder;
  generator->year = year;
}

inline Array *CGenerator_getOutputBuffer(CGenerator *generator, uint32_t index) {
  if (index >= 16) { return nullptr; }
  if (!generator->buffers[index]) {
    generator->buffers[index] =
        Array_new(sizeof(char_t), INT32_MAX, generator->__SUPPER__.allocator);
  }
  return generator->buffers[index];
}

inline AVLTree *Elf64Generator_getInstrFormTree(Elf64Generator *generator) {
  return generator->instr_form_tree;
}
inline AVLTree *Elf64Generator_getEnumTree(Elf64Generator *generator) {
  return generator->enum_tree;
}
inline Array *Elf64Generator_getEnumArray(Elf64Generator *generator) {
  return generator->enum_array;
}
inline Elf64Compositor *Elf64Generator_getCompositor(Elf64Generator *generator) {
  return generator->compositor;
}

inline void Elf64Generator_set_cose(Elf64Generator *generator, uint32_t val) {
  generator->count_of_single_entries = val;
}

inline void Elf64Generator_set_cote(Elf64Generator *generator, uint32_t val) {
  generator->count_of_total_entries = val;
}

uint32_t Elf64Generator_get_sym_index(Elf64Generator *, uint64_t) {
  return 0;
}

void Generator_destroy(Generator *generator) {
  switch (generator->gen_type) {
    case GT_C: {
      CGenerator *c_generator = (CGenerator *) generator;
      for (uint32_t i = 0; i < 16; i++) {
        if (c_generator->buffers[i]) { releasePrimeArray(c_generator->buffers[i]); }
      }
      break;
    }
    case GT_X86_64: {
      Elf64Generator *elf64_generator = (Elf64Generator *) generator;
      if (elf64_generator->enum_array) { releasePrimeArray(elf64_generator->enum_array); }
      if (elf64_generator->enum_tree) { AVLTree_destroy(elf64_generator->enum_tree, nullptr); }
      if (elf64_generator->compositor) { Elf64Compositor_destroy(elf64_generator->compositor); }
      break;
    }
  }
  generator->allocator->free(generator);
}
