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
 * Module Name: codegen/elf/x64
 * Filename: Elf64Compositor.h
 * Creator: Yaokai Liu
 * Create Date: 2025-03-03
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_COMPOSITOR_H
#define MACHINE_COMPOSITOR_H

#include "array.h"
#include "char_t.h"
#include "trie.h"
#include <elf.h>

enum ELF_SEC_TYPE_ENUM : uint8_t {
  enum_elf_NONE = 0,

  enum_elf_shdr,
  enum_elf_phdr,
  enum_elf_str,
  enum_elf_sym,
  enum_elf_rel,
  enum_elf_rela,
  enum_elf_bss,
  enum_elf_data,
  enum_elf_text,
  enum_elf_rodata,
  enum_elf_section,
};

extern const uint8_t RODATA_TAG[];
extern const uint8_t SYMTAB_TAG[];
extern const uint8_t STRTAB_TAG[];
extern const uint8_t REL_RODATA_TAG[];

typedef struct Elf64Compositor {
  const Allocator *allocator;
  Elf64_Ehdr *elf_header;
  Trie /*<uint8_t, REFER(Array)>*/ *sec_trie;
  Array /*<Elf64_Phdr>*/ *phdr_array;
  Array /*<Elf64_Shdr>*/ *shdr_array;
  Array /*<Array>*/ *sections;
} Elf64Compositor;

Elf64Compositor *Elf64Compositor_new_sysv_2lsb(
    Elf64_Half e_type, Elf32_Half e_machine, const Allocator *allocator
);

void Elf64Compositor_destroy(Elf64Compositor *compositor);

uint32_t Elf64_get_sec_index(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag);
Array *Elf64_get_section(Elf64Compositor *compositor, const uint8_t *tag);
Array *Elf64_new_section(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag, char_t *name);
Elf64_Shdr *Elf64_get_shdr(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag);
uint32_t Elf64_curr_offset(Elf64Compositor *compositor, uint8_t *tag);

#endif  // MACHINE_COMPOSITOR_H
