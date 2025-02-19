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
 * Filename: elf64.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-19
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ELF64_H
#define MACHINE_ELF64_H

#include <stdint.h>

typedef uint64_t Elf64_Addr;
typedef uint16_t Elf64_Half;
typedef uint64_t Elf64_Off;
typedef int32_t Elf64_Sword;
typedef uint32_t Elf64_Word;
typedef int64_t Elf64_Sxword;
typedef uint64_t Elf64_Xword;
typedef uint8_t Elf64_Char;

enum E_TYPE {
  ET_NONE = 0,
  ET_REL = 1,
  ET_EXEC = 2,
  ET_DYN = 3,
  ET_CORE = 4,
  ET_LOOS = 0xfe00,
  ET_HIOS = 0xfeff,
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

enum E_IDENT_OSABI {
  EIO_SYSV = 0,
  EIO_HPUX = 1,
  EIO_STANDALONE = 0xff,
};

typedef struct {
  unsigned char e_ident[16]; /* ELF identification */
  Elf64_Half e_type; /* Object file type */
  Elf64_Half e_machine; /* Machine type */
  Elf64_Word e_version; /* Object file version */
  Elf64_Addr e_entry; /* Entry point address */
  Elf64_Off e_phoff; /* Program header offset */
  Elf64_Off e_shoff; /* Section header offset */
  Elf64_Word e_flags; /* Processor-specific flags */
  Elf64_Half e_ehsize; /* ELF header size */
  Elf64_Half e_phentsize; /* Size of program header entry */
  Elf64_Half e_phnum; /* Number of program header entries */
  Elf64_Half e_shentsize; /* Size of section header entry */
  Elf64_Half e_shnum; /* Number of section header entries */
  Elf64_Half e_shstrndx; /* Section name string table index */
} Elf64_Ehdr;

enum SH_INDEX {
  SHN_UNDEF = 0,
  SHN_LORESERVE = 0xff00,
  SHN_LOPROC = 0xff00,
  SHN_HIPROC = 0xff1f,
  SHN_LOOS = 0xff20,
  SHN_HIOS = 0xff3f,
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
  SHT_LOOS = 0x60000000,
  SHT_HIOS = 0x6fffffff,
  SHT_LOPROC = 0x70000000,
  SHT_HIPROC = 0x7fffffff,
  SHT_LOUSER = 0x80000000,
  SHT_HIUSER = 0xffffffff,
};

enum SH_FLAG {
  SHF_WRITE = 0x1,
  SHF_ALLOC = 0x2,
  SHF_EXECINSTR = 0x4,
  SHF_MASKOS = 0x0f000000,
  SHF_MASKPROC = 0xf0000000,
};

typedef struct {
  Elf64_Word sh_name; /* Section name */
  Elf64_Word sh_type; /* Section type */
  Elf64_Xword sh_flags; /* Section attributes */
  Elf64_Addr sh_addr; /* Virtual address in memory */
  Elf64_Off sh_offset; /* Offset in file */
  Elf64_Xword sh_size; /* Size of section */
  Elf64_Word sh_link; /* Link to other section */
  Elf64_Word sh_info; /* Miscellaneous information */
  Elf64_Xword sh_addralign; /* Address alignment boundary */
  Elf64_Xword sh_entsize; /* Size of entries, if section has table */
} Elf64_Shdr;

enum ST_BIND {
  STB_LOCAL = 0,
  STB_GLOBAL = 1,
  STB_WEAK = 2,
  STB_LOOS = 10,
  STB_HIOS = 12,
  STB_LOPROC = 13,
  STB_HIPROC = 15,
};
enum ST_TYPE {
  STT_NONE = 0,
  STT_OBJECT = 1,
  STT_FUNC = 2,
  STT_SECTION = 3,
  STT_FILE = 4,
  STT_LOOS = 10,
  STT_HIOS = 12,
  STT_LOPROC = 13,
  STT_HIPROC = 15,
};

typedef struct {
  Elf64_Word st_name; /* Symbol name */
  unsigned char st_info; /* Type and Binding attributes */
  unsigned char st_other; /* Reserved */
  Elf64_Half st_shndx; /* Section table index */
  Elf64_Addr st_value; /* Symbol value */
  Elf64_Xword st_size; /* Size of object (e.g., common) */
} Elf64_Sym;

#define ELF64_R_SYM(i)     ((i) >> 32)
#define ELF64_R_TYPE(i)    ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))

typedef struct {
  Elf64_Addr r_offset; /* Address of reference */
  Elf64_Xword r_info; /* Symbol index and type of relocation */
} Elf64_Rel;

typedef struct {
  Elf64_Addr r_offset; /* Address of reference */
  Elf64_Xword r_info; /* Symbol index and type of relocation */
  Elf64_Sxword r_addend; /* Constant part of expression */
} Elf64_Rela;

enum P_TYPE {
  PT_NULL = 0,
  PT_LOAD = 0,
  PT_DYNAMIC = 0,
  PT_INTERP = 0,
  PT_NOTE = 0,
  PT_SHLIB = 0,
  PT_PHDR = 0,
  PT_LOOS = 0x60000000,
  PT_HIOS = 0x6fffffff,
  PT_LOPROC = 0x70000000,
  PT_HIPROC = 0x7fffffff,
};

enum P_FLAG {
  PF_X = 0x1,
  PF_W = 0x2,
  PF_R = 0x4,
  PF_MASKOS = 0x00ff0000,
  PF_MASKPROC = 0xff000000,
};

typedef struct {
  Elf64_Word p_type; /* Type of segment */
  Elf64_Word p_flags; /* Segment attributes */
  Elf64_Off p_offset; /* Offset in file */
  Elf64_Addr p_vaddr; /* Virtual address in memory */
  Elf64_Addr p_paddr; /* Reserved */
  Elf64_Xword p_filesz; /* Size of segment in file */
  Elf64_Xword p_memsz; /* Size of segment in memory */
  Elf64_Xword p_align; /* Alignment of segment */
} Elf64_Phdr;


enum D_TAG_TYPE {
    DT_NULL,
    DT_NEEDED,

    DT_PLTRELSZ,

    DT_PLTGOT,

    DT_HASH,
    DT_STRTAB,
    DT_SYMTAB,
    DT_RELA,
    DT_RELASZ,
    DT_RELAENT,
    DT_STRSZ,
    DT_SYMENT,
    DT_INIT,
    DT_FINI,
    DT_SONAME,
    DT_RPATH,
    DT_SYMBOLIC,

    DT_REL,
    DT_RELSZ,
    DT_RELENT,
    DT_PLTREL,

    DT_DEBUG,
    DT_TEXTREL,

    DT_JUMPREL,

    DT_BIND_NOW,

    DT_INIT_ARRAY,
    DT_FINI_ARRAY,
    DT_INIT_ARRAYSZ,
    DT_FINI_ARRAYSZ,

    DT_LOOS = 0x60000000,
    DT_HIOS = 0x6fffffff,
    DT_LOPROC = 0x70000000,
    DT_HIPROC = 0x7fffffff,
};

typedef struct {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;
extern Elf64_Dyn _DYNAMIC[]

#endif  // MACHINE_ELF64_H
