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
 * Module Name: codegen/elf
 * Filename: elf.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-19
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ELF_H
#define MACHINE_ELF_H

#include <stdint.h>

typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef int32_t Elf32_Sword;
typedef uint32_t Elf32_Word;
typedef uint8_t Elf32_Char;

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef int32_t  Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef int64_t  Elf64_Sxword;
typedef uint64_t Elf64_Xword;
typedef uint8_t  Elf64_Char;

enum E_TYPE {
  ET_NONE = 0,
  ET_REL = 1,
  ET_EXEC = 2,
  ET_DYN = 3,
  ET_CORE = 4,
  ET_LOPROC = 0xff00,
  ET_HIPROC = 0xffff,
};

enum E_MACHINE {
  EM_NONE = ET_NONE,
  EM_M32,
  EM_SPARC,
  EM_386,
  EM_68K,
  EM_88K,
  EM_860,
  EM_MIPS,
  EM_MIPS_RS4_BE,
  EM_RESERVED,
};

enum E_VERSION {
  EV_NONE = ET_NONE,
  EV_CURRENT = 1,
};

enum E_IDENT {
  EI_MAG0 = 0,
  EI_MAG1 = 1,
  EI_MAG2 = 2,
  EI_MAG3 = 3,
  EI_CLASS = 4,
  EI_DATA = 5,
  EI_VERSION = 6,
  EI_OSABI = 7,
  EI_ABIVERSION = 8,
  EI_PAD = 9,
  EI_NIDENT = 16,
};

enum E_IDENT_MAGIC : uint8_t {
  EIM_0 = 0x7f,
  EIM_1 = 'e',
  EIM_2 = 'l',
  EIM_3 = 'f',
};

enum E_IDENT_CLASS {
  EIC_NONE = 0,
  EIC_32 = 1,
  EIC_64 = 2,
};

enum E_IDENT_DATA {
  EID_NONE = 0,
  EID_LSB = 1,
  EID_MSB = 2,
};

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf32_Half e_type;
  Elf32_Half e_machine;
  Elf32_Word e_version;
  Elf32_Addr e_entry;
  Elf32_Off e_phoff;
  Elf32_Off e_shoff;
  Elf32_Word e_flags;
  Elf32_Half e_ehsize;
  Elf32_Half e_phentsize;
  Elf32_Half e_phnum;
  Elf32_Half e_shentsize;
  Elf32_Half e_shnum;
  Elf32_Half e_shstrndx;
} Elf32_Ehdr;

typedef struct {
  unsigned char e_ident[EI_NIDENT];
  Elf64_Half e_type;
  Elf64_Half e_machine;
  Elf64_Word e_version;
  Elf64_Addr e_entry;
  Elf64_Off e_phoff;
  Elf64_Off e_shoff;
  Elf64_Word e_flags;
  Elf64_Half e_ehsize;
  Elf64_Half e_phentsize;
  Elf64_Half e_phnum;
  Elf64_Half e_shentsize;
  Elf64_Half e_shnum;
  Elf64_Half e_shstrndx;
} Elf64_Ehdr;

enum SH_INDEX {
  SHN_UNDEF = 0,
  SHN_LORESERVE = 0xff00,
  SHN_LOPROC = 0xff00,
  SHN_HIPROC = 0xff1f,
  SHN_ABS = 0xfff1,
  SHN_COMMON = 0xfff2,
  SHN_HIRESERVE = 0xffff,
};

enum SH_TYPE {
  SHT_NULL = 0,
  SHT_PROGBITS = 1,
  SHT_SYMTAB = 2,
  SHT_STRTAB = 3,
  SHT_RELA = 4,
  SHT_HASH = 5,
  SHT_DYNAMIC = 6,
  SHT_NOTE = 7,
  SHT_NOBITS = 8,
  SHT_REL = 9,
  SHT_SHLIB = 10,
  SHT_DYNSYM = 11,
  SHT_LOPROC = 0x70000000,
  SHT_HIPROC = 0x7fffffff,
  SHT_LOUSER = 0x80000000,
  SHT_HIUSER = 0xffffffff,
};

enum SH_FLAG {
    SHF_WRITE = 0x1,
    SHF_ALLOC = 0x2,
    SHF_EXECINSTR = 0x4,
    SHF_MASKPROC = 0xf0000000,
};

typedef struct {
  Elf32_Word sh_name;
  Elf32_Word sh_type;
  Elf32_Word sh_flags;
  Elf32_Addr sh_addr;
  Elf32_Off sh_offset;
  Elf32_Word sh_size;
  Elf32_Word sh_link;
  Elf32_Word sh_info;
  Elf32_Word sh_addralign;
  Elf32_Word sh_entsize;
} Elf32_Shdr;

typedef struct {
    Elf32_Word st_name;
    Elf32_Addr st_value;
    Elf32_Word st_size;
    unsigned char st_info;
    unsigned char st_other;
    Elf32_Half st_shndx;
} Elf32_Sym;

#endif  // MACHINE_ELF_H
