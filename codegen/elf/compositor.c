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
 * Filename: Elf64Compositor.c
 * Creator: Yaokai Liu
 * Create Date: 2025-03-03
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "compositor.h"
#include <string.h>

const uint8_t TEXT_TAG[] = {enum_elf_text, 0};
const uint8_t RODATA_TAG[] = {enum_elf_rodata, 0};
const uint8_t SYMTAB_TAG[] = {enum_elf_sym, 0};
const uint8_t STRTAB_TAG[] = {enum_elf_str, 0};
const uint8_t REL_RODATA_TAG[] = {enum_elf_rel, enum_elf_rodata, 0};

const uint8_t SEC_ENTRY_SIZES[] = {
    [enum_elf_shdr] = sizeof(Elf64_Shdr), [enum_elf_phdr] = sizeof(Elf64_Phdr),
    [enum_elf_str] = sizeof(char_t),      [enum_elf_sym] = sizeof(Elf64_Sym),
    [enum_elf_rel] = sizeof(Elf64_Rel),   [enum_elf_rela] = sizeof(Elf64_Rela),
    [enum_elf_bss] = sizeof(uint8_t),     [enum_elf_data] = sizeof(uint8_t),
    [enum_elf_text] = sizeof(uint8_t),    [enum_elf_rodata] = sizeof(uint8_t),
};
const uint8_t SEC_TYPES[] = {
    [enum_elf_shdr] = 0,
    [enum_elf_phdr] = 0,
    [enum_elf_str] = SHT_STRTAB,
    [enum_elf_sym] = SHT_SYMTAB,
    [enum_elf_rel] = SHT_REL,
    [enum_elf_rela] = SHT_RELA,
    [enum_elf_bss] = SHT_NOBITS,
    [enum_elf_data] = SHT_PROGBITS,
    [enum_elf_text] = SHT_PROGBITS,
    [enum_elf_rodata] = SHT_PROGBITS,
};
const uint8_t SEC_FLAGS[] = {
    [enum_elf_shdr] = 0,
    [enum_elf_phdr] = 0,
    [enum_elf_str] = 0,
    [enum_elf_sym] = SHF_ALLOC,
    [enum_elf_rel] = SHF_ALLOC,
    [enum_elf_rela] = 0,
    [enum_elf_bss] = SHF_ALLOC | SHF_WRITE,
    [enum_elf_data] = SHF_ALLOC | SHF_WRITE,
    [enum_elf_text] = SHF_ALLOC | SHF_EXECINSTR,
    [enum_elf_rodata] = SHF_ALLOC,
};

#define ElfHeader_init_ident(e_ident, class, data, os_abi) \
  do {                                                     \
    e_ident[EI_MAG0] = ELFMAG0;                            \
    e_ident[EI_MAG1] = ELFMAG1;                            \
    e_ident[EI_MAG2] = ELFMAG2;                            \
    e_ident[EI_MAG3] = ELFMAG3;                            \
    e_ident[EI_CLASS] = class;                             \
    e_ident[EI_DATA] = data;                               \
    e_ident[EI_VERSION] = EV_CURRENT;                      \
    e_ident[EI_OSABI] = os_abi;                            \
  } while (false)

#define ElfStrTable_push_string(strtab, string) \
  do { Array_append(strtab, string, strlen(string) + 1); } while (false)

void Elf64_add_section(Elf64Compositor *compositor, Elf64_Shdr *shdr, Array *section);

Elf64Compositor *Elf64Compositor_new_sysv_2lsb(
    Elf64_Half e_type, Elf32_Half e_machine, const Allocator *allocator
) {
  Elf64Compositor *compositor = allocator->calloc(1, sizeof(compositor));
  compositor->allocator = allocator;
  compositor->elf_header = allocator->calloc(1, sizeof(Elf64_Ehdr));
  ElfHeader_init_ident(compositor->elf_header->e_ident, ELFCLASS64, ELFDATA2LSB, ELFOSABI_SYSV);
  compositor->elf_header->e_type = e_type;
  compositor->elf_header->e_machine = e_machine;
  compositor->elf_header->e_version = EV_CURRENT;

  Elf64_Shdr shdr = {};
  compositor->shdr_array = Array_new(sizeof(Elf64_Shdr), enum_elf_shdr, allocator);
  Array_append(compositor->shdr_array, &shdr, 1);
  compositor->sections = Array_new(sizeof_array, enum_elf_section, allocator);

  shdr.sh_name = 1;
  shdr.sh_type = SHT_STRTAB;
  shdr.sh_flags = SHF_ALLOC | SHF_WRITE;
  Array *shstrtab = Array_new(sizeof(char_t), enum_elf_str, allocator);
  ElfStrTable_push_string(shstrtab, "");
  ElfStrTable_push_string(shstrtab, ".shstrtab");
  Elf64_add_section(compositor, &shdr, shstrtab);
  Array_destroy(shstrtab);

  Elf64_new_section(compositor, (uint8_t *) RODATA_TAG, ".rodata");
  Elf64_Shdr *rodata_shdr = Elf64_get_shdr(compositor, (uint8_t *) RODATA_TAG);
  rodata_shdr->sh_link = SHN_UNDEF;
  rodata_shdr->sh_addralign = 16;
  rodata_shdr->sh_entsize = 0;
  rodata_shdr->sh_info = 0;

  Elf64_new_section(compositor, (uint8_t *) SYMTAB_TAG, ".symtab");
  Elf64_Shdr *sym_shdr = Elf64_get_shdr(compositor, (uint8_t *) SYMTAB_TAG);
  sym_shdr->sh_entsize = sizeof(Elf64_Sym);
  sym_shdr->sh_link = SHN_UNDEF;
  sym_shdr->sh_addralign = 16;
  sym_shdr->sh_info = 0;

  Elf64_new_section(compositor, (uint8_t *) STRTAB_TAG, ".strtab");
  Elf64_Shdr *str_shdr = Elf64_get_shdr(compositor, (uint8_t *) STRTAB_TAG);
  str_shdr->sh_link = SHN_UNDEF;
  str_shdr->sh_addralign = 16;
  str_shdr->sh_entsize = 0;
  str_shdr->sh_info = 0;

  Elf64_new_section(compositor, (uint8_t *) REL_RODATA_TAG, ".rel.rodata");
  Elf64_Shdr *rel_shdr = Elf64_get_shdr(compositor, (uint8_t *) REL_RODATA_TAG);
  rel_shdr->sh_link = Elf64_get_sec_index(compositor, (uint8_t *) RODATA_TAG);
  rel_shdr->sh_entsize = sizeof(Elf64_Rel);
  rel_shdr->sh_addralign = 16;
  rel_shdr->sh_info = 0;

  return compositor;
}

inline void Elf64Compositor_destroy(Elf64Compositor *compositor) {
  if (compositor->elf_header) { compositor->allocator->free(compositor->elf_header); }
  if (compositor->phdr_array) { releasePrimeArray(compositor->phdr_array); }
  if (compositor->shdr_array) { releasePrimeArray(compositor->shdr_array); }
  releasePrimeArray(compositor->shdr_array);
  const uint32_t n_sections = Array_length(compositor->sections);
  Array *sections = Array_first_real(compositor->sections);
  for (uint32_t i = 0; i < n_sections; i++) {
    Array_reset(((void *) sections) + i * sizeof_array, nullptr);
  }
  releasePrimeArray(compositor->sections);
}

inline void Elf64_add_section(Elf64Compositor *compositor, Elf64_Shdr *shdr, Array *section) {
  Array_append(compositor->shdr_array, shdr, 1);
  Array_append(compositor->sections, section, 1);
}

inline Array *Elf64_get_section(Elf64Compositor *compositor, const uint8_t *tag) {
  REFER(Array) v_section = Trie_get(compositor->sec_trie, tag);
  if (v_section) { return Array_virt2real(compositor->sections, v_section); }
  return nullptr;
}

#define Elf64_get_shstrtab(dest)                                         \
  do {                                                                   \
    const uint8_t _shstrtab_tag[] = {enum_elf_str, enum_elf_section, 0}; \
    dest = Elf64_get_section(compositor, _shstrtab_tag);                 \
  } while (0)

inline Array *
    Elf64_new_section(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag, char_t *name) {
  REFER(Array) v_section = Trie_get(compositor->sec_trie, tag);
  if (v_section) { return Array_virt2real(compositor->sections, v_section); }
  Array *shstrtab;
  Elf64_get_shstrtab(shstrtab);
  Elf64_Shdr shdr = {
      .sh_name = Array_length(shstrtab),
      .sh_type = SEC_TYPES[tag[0]],
      .sh_flags = SEC_FLAGS[tag[0]],
      .sh_entsize = SEC_ENTRY_SIZES[tag[0]],
  };
  ElfStrTable_push_string(shstrtab, name);
  Array *section = Array_new(SEC_ENTRY_SIZES[tag[0]], tag[0], compositor->allocator);
  Elf64_add_section(compositor, &shdr, section);
  Array_destroy(shstrtab);
  return Array_last_real(compositor->sections);
}

#define Elf64_vsec_to_shdr(v_section) \
  Array_real_addr(compositor->shdr_array, ((uint32_t) (uint64_t) v_section) & 0xffffffff)

inline Elf64_Shdr *Elf64_get_shdr(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag) {
  REFER(Array) v_section = Trie_get(compositor->sec_trie, tag);
  if (v_section) { return Elf64_vsec_to_shdr(v_section); }
  return nullptr;
}
inline uint32_t Elf64_get_sec_index(Elf64Compositor *compositor, enum ELF_SEC_TYPE_ENUM *tag) {
  REFER(Array) v_section = Trie_get(compositor->sec_trie, tag);
  if (v_section) { return ((uint32_t) (uint64_t) v_section) & 0xffffffff; }
  return 0;
}

inline uint32_t Elf64_curr_offset(Elf64Compositor *compositor, uint8_t *tag) {
  Array *section = Elf64_get_section(compositor, tag);
  return Array_length(section) * SEC_ENTRY_SIZES[tag[0]];
}
