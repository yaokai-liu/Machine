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
  ET_NONE = 0,        /* No file type */
  ET_REL = 1,         /* Relocatable file */
  ET_EXEC = 2,        /* Executable file */
  ET_DYN = 3,         /* Shared object file */
  ET_CORE = 4,        /* Core file */
  ET_NUM = 5,         /* Number of defined types */
  ET_LOOS = 0xfe00,   /* OS-specific range start */
  ET_HIOS = 0xfeff,   /* OS-specific range end */
  ET_LOPROC = 0xff00, /* Processor-specific range start */
  ET_HIPROC = 0xffff, /* Processor-specific range end */
};

enum E_MACHINE {
  EM_NONE = 0,         /* No machine */
  EM_M32 = 1,          /* AT&T WE 32100 */
  EM_SPARC = 2,        /* SUN SPARC */
  EM_386 = 3,          /* Intel 80386 */
  EM_68K = 4,          /* Motorola m68k family */
  EM_88K = 5,          /* Motorola m88k family */
  EM_IAMCU = 6,        /* Intel MCU */
  EM_860 = 7,          /* Intel 80860 */
  EM_MIPS = 8,         /* MIPS R3000 big-endian */
  EM_S370 = 9,         /* IBM System/370 */
  EM_MIPS_RS3_LE = 10, /* MIPS R3000 little-endian */
  /* reserved 11-14 */
  EM_PARISC = 15, /* HPPA */
  /* reserved 16 */
  EM_VPP500 = 17,      /* Fujitsu VPP500 */
  EM_SPARC32PLUS = 18, /* Sun's "v8plus" */
  EM_960 = 19,         /* Intel 80960 */
  EM_PPC = 20,         /* PowerPC */
  EM_PPC64 = 21,       /* PowerPC 64-bit */
  EM_S390 = 22,        /* IBM S390 */
  EM_SPU = 23,         /* IBM SPU/SPC */
  /* reserved 24-35 */
  EM_V800 = 36,          /* NEC V800 series */
  EM_FR20 = 37,          /* Fujitsu FR20 */
  EM_RH32 = 38,          /* TRW RH-32 */
  EM_RCE = 39,           /* Motorola RCE */
  EM_ARM = 40,           /* ARM */
  EM_FAKE_ALPHA = 41,    /* Digital Alpha */
  EM_SH = 42,            /* Hitachi SH */
  EM_SPARCV9 = 43,       /* SPARC v9 64-bit */
  EM_TRICORE = 44,       /* Siemens Tricore */
  EM_ARC = 45,           /* Argonaut RISC Core */
  EM_H8_300 = 46,        /* Hitachi H8/300 */
  EM_H8_300H = 47,       /* Hitachi H8/300H */
  EM_H8S = 48,           /* Hitachi H8S */
  EM_H8_500 = 49,        /* Hitachi H8/500 */
  EM_IA_64 = 50,         /* Intel Merced */
  EM_MIPS_X = 51,        /* Stanford MIPS-X */
  EM_COLDFIRE = 52,      /* Motorola Coldfire */
  EM_68HC12 = 53,        /* Motorola M68HC12 */
  EM_MMA = 54,           /* Fujitsu MMA Multimedia Accelerator */
  EM_PCP = 55,           /* Siemens PCP */
  EM_NCPU = 56,          /* Sony nCPU embeeded RISC */
  EM_NDR1 = 57,          /* Denso NDR1 microprocessor */
  EM_STARCORE = 58,      /* Motorola Start*Core processor */
  EM_ME16 = 59,          /* Toyota ME16 processor */
  EM_ST100 = 60,         /* STMicroelectronic ST100 processor */
  EM_TINYJ = 61,         /* Advanced Logic Corp. Tinyj emb.fam */
  EM_X86_64 = 62,        /* AMD x86-64 architecture */
  EM_PDSP = 63,          /* Sony DSP Processor */
  EM_PDP10 = 64,         /* Digital PDP-10 */
  EM_PDP11 = 65,         /* Digital PDP-11 */
  EM_FX66 = 66,          /* Siemens FX66 microcontroller */
  EM_ST9PLUS = 67,       /* STMicroelectronics ST9+ 8/16 mc */
  EM_ST7 = 68,           /* STmicroelectronics ST7 8 bit mc */
  EM_68HC16 = 69,        /* Motorola MC68HC16 microcontroller */
  EM_68HC11 = 70,        /* Motorola MC68HC11 microcontroller */
  EM_68HC08 = 71,        /* Motorola MC68HC08 microcontroller */
  EM_68HC05 = 72,        /* Motorola MC68HC05 microcontroller */
  EM_SVX = 73,           /* Silicon Graphics SVx */
  EM_ST19 = 74,          /* STMicroelectronics ST19 8 bit mc */
  EM_VAX = 75,           /* Digital VAX */
  EM_CRIS = 76,          /* Axis Communications 32-bit emb.proc */
  EM_JAVELIN = 77,       /* Infineon Technologies 32-bit emb.proc */
  EM_FIREPATH = 78,      /* Element 14 64-bit DSP Processor */
  EM_ZSP = 79,           /* LSI Logic 16-bit DSP Processor */
  EM_MMIX = 80,          /* Donald Knuth's educational 64-bit proc */
  EM_HUANY = 81,         /* Harvard University machine-independent object files */
  EM_PRISM = 82,         /* SiTera Prism */
  EM_AVR = 83,           /* Atmel AVR 8-bit microcontroller */
  EM_FR30 = 84,          /* Fujitsu FR30 */
  EM_D10V = 85,          /* Mitsubishi D10V */
  EM_D30V = 86,          /* Mitsubishi D30V */
  EM_V850 = 87,          /* NEC v850 */
  EM_M32R = 88,          /* Mitsubishi M32R */
  EM_MN10300 = 89,       /* Matsushita MN10300 */
  EM_MN10200 = 90,       /* Matsushita MN10200 */
  EM_PJ = 91,            /* picoJava */
  EM_OPENRISC = 92,      /* OpenRISC 32-bit embedded processor */
  EM_ARC_COMPACT = 93,   /* ARC International ARCompact */
  EM_XTENSA = 94,        /* Tensilica Xtensa Architecture */
  EM_VIDEOCORE = 95,     /* Alphamosaic VideoCore */
  EM_TMM_GPP = 96,       /* Thompson Multimedia General Purpose Proc */
  EM_NS32K = 97,         /* National Semi. 32000 */
  EM_TPC = 98,           /* Tenor Network TPC */
  EM_SNP1K = 99,         /* Trebia SNP 1000 */
  EM_ST200 = 100,        /* STMicroelectronics ST200 */
  EM_IP2K = 101,         /* Ubicom IP2xxx */
  EM_MAX = 102,          /* MAX processor */
  EM_CR = 103,           /* National Semi. CompactRISC */
  EM_F2MC16 = 104,       /* Fujitsu F2MC16 */
  EM_MSP430 = 105,       /* Texas Instruments msp430 */
  EM_BLACKFIN = 106,     /* Analog Devices Blackfin DSP */
  EM_SE_C33 = 107,       /* Seiko Epson S1C33 family */
  EM_SEP = 108,          /* Sharp embedded microprocessor */
  EM_ARCA = 109,         /* Arca RISC */
  EM_UNICORE = 110,      /* PKU-Unity & MPRC Peking Uni. mc series */
  EM_EXCESS = 111,       /* eXcess configurable cpu */
  EM_DXP = 112,          /* Icera Semi. Deep Execution Processor */
  EM_ALTERA_NIOS2 = 113, /* Altera Nios II */
  EM_CRX = 114,          /* National Semi. CompactRISC CRX */
  EM_XGATE = 115,        /* Motorola XGATE */
  EM_C166 = 116,         /* Infineon C16x/XC16x */
  EM_M16C = 117,         /* Renesas M16C */
  EM_DSPIC30F = 118,     /* Microchip Technology dsPIC30F */
  EM_CE = 119,           /* Freescale Communication Engine RISC */
  EM_M32C = 120,         /* Renesas M32C */
  /* reserved 121-130 */
  EM_TSK3000 = 131,       /* Altium TSK3000 */
  EM_RS08 = 132,          /* Freescale RS08 */
  EM_SHARC = 133,         /* Analog Devices SHARC family */
  EM_ECOG2 = 134,         /* Cyan Technology eCOG2 */
  EM_SCORE7 = 135,        /* Sunplus S+core7 RISC */
  EM_DSP24 = 136,         /* New Japan Radio (NJR) 24-bit DSP */
  EM_VIDEOCORE3 = 137,    /* Broadcom VideoCore III */
  EM_LATTICEMICO32 = 138, /* RISC for Lattice FPGA */
  EM_SE_C17 = 139,        /* Seiko Epson C17 */
  EM_TI_C6000 = 140,      /* Texas Instruments TMS320C6000 DSP */
  EM_TI_C2000 = 141,      /* Texas Instruments TMS320C2000 DSP */
  EM_TI_C5500 = 142,      /* Texas Instruments TMS320C55x DSP */
  EM_TI_ARP32 = 143,      /* Texas Instruments App. Specific RISC */
  EM_TI_PRU = 144,        /* Texas Instruments Prog. Realtime Unit */
  /* reserved 145-159 */
  EM_MMDSP_PLUS = 160,  /* STMicroelectronics 64bit VLIW DSP */
  EM_CYPRESS_M8C = 161, /* Cypress M8C */
  EM_R32C = 162,        /* Renesas R32C */
  EM_TRIMEDIA = 163,    /* NXP Semi. TriMedia */
  EM_QDSP6 = 164,       /* QUALCOMM DSP6 */
  EM_8051 = 165,        /* Intel 8051 and variants */
  EM_STXP7X = 166,      /* STMicroelectronics STxP7x */
  EM_NDS32 = 167,       /* Andes Tech. compact code emb. RISC */
  EM_ECOG1X = 168,      /* Cyan Technology eCOG1X */
  EM_MAXQ30 = 169,      /* Dallas Semi. MAXQ30 mc */
  EM_XIMO16 = 170,      /* New Japan Radio (NJR) 16-bit DSP */
  EM_MANIK = 171,       /* M2000 Reconfigurable RISC */
  EM_CRAYNV2 = 172,     /* Cray NV2 vector architecture */
  EM_RX = 173,          /* Renesas RX */
  EM_METAG = 174,       /* Imagination Tech. META */
  EM_MCST_ELBRUS = 175, /* MCST Elbrus */
  EM_ECOG16 = 176,      /* Cyan Technology eCOG16 */
  EM_CR16 = 177,        /* National Semi. CompactRISC CR16 */
  EM_ETPU = 178,        /* Freescale Extended Time Processing Unit */
  EM_SLE9X = 179,       /* Infineon Tech. SLE9X */
  EM_L10M = 180,        /* Intel L10M */
  EM_K10M = 181,        /* Intel K10M */
  /* reserved 182 */
  EM_AARCH64 = 183, /* ARM AARCH64 */
  /* reserved 184 */
  EM_AVR32 = 185,        /* Amtel 32-bit microprocessor */
  EM_STM8 = 186,         /* STMicroelectronics STM8 */
  EM_TILE64 = 187,       /* Tileta TILE64 */
  EM_TILEPRO = 188,      /* Tilera TILEPro */
  EM_MICROBLAZE = 189,   /* Xilinx MicroBlaze */
  EM_CUDA = 190,         /* NVIDIA CUDA */
  EM_TILEGX = 191,       /* Tilera TILE-Gx */
  EM_CLOUDSHIELD = 192,  /* CloudShield */
  EM_COREA_1ST = 193,    /* KIPO-KAIST Core-A 1st gen. */
  EM_COREA_2ND = 194,    /* KIPO-KAIST Core-A 2nd gen. */
  EM_ARC_COMPACT2 = 195, /* Synopsys ARCompact V2 */
  EM_OPEN8 = 196,        /* Open8 RISC */
  EM_RL78 = 197,         /* Renesas RL78 */
  EM_VIDEOCORE5 = 198,   /* Broadcom VideoCore V */
  EM_78KOR = 199,        /* Renesas 78KOR */
  EM_56800EX = 200,      /* Freescale 56800EX DSC */
  EM_BA1 = 201,          /* Beyond BA1 */
  EM_BA2 = 202,          /* Beyond BA2 */
  EM_XCORE = 203,        /* XMOS xCORE */
  EM_MCHP_PIC = 204,     /* Microchip 8-bit PIC(r) */

  /* reserved 205-209 */

  EM_KM32 = 210,        /* KM211 KM32 */
  EM_KMX32 = 211,       /* KM211 KMX32 */
  EM_EMX16 = 212,       /* KM211 KMX16 */
  EM_EMX8 = 213,        /* KM211 KMX8 */
  EM_KVARC = 214,       /* KM211 KVARC */
  EM_CDP = 215,         /* Paneve CDP */
  EM_COGE = 216,        /* Cognitive Smart Memory Processor */
  EM_COOL = 217,        /* Bluechip CoolEngine */
  EM_NORC = 218,        /* Nanoradio Optimized RISC */
  EM_CSR_KALIMBA = 219, /* CSR Kalimba */
  EM_Z80 = 220,         /* Zilog Z80 */
  EM_VISIUM = 221,      /* Controls and Data Services VISIUMcore */
  EM_FT32 = 222,        /* FTDI Chip FT32 */
  EM_MOXIE = 223,       /* Moxie processor */
  EM_AMDGPU = 224,      /* AMD GPU */

  /* reserved 225-242 */

  EM_RISCV = 243, /* RISC-V */
  EM_BPF = 247,   /* Linux BPF -- in-kernel virtual machine */
  EM_CSKY = 252,  /* C-SKY */
  EM_NUM = 253,   /* Old spellings/synonyms.  */

  EM_ARC_A5 = EM_ARC_COMPACT,

  /* If it is necessary to assign new unofficial EM_* values, please
     pick large random numbers (0x8523, 0xa7f2, etc.) to minimize the
     chances of collision with official or non-GNU unofficial values.  */

  EM_ALPHA = 0x9026
};

enum E_VERSION {
  EV_NONE = ET_NONE,
  EV_CURRENT = 1,
  EV_NUM = 2
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
  EIM_1 = 'E',
  EIM_2 = 'L',
  EIM_3 = 'F',
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
  Elf64_Half e_type;         /* Object file type */
  Elf64_Half e_machine;      /* Machine type */
  Elf64_Word e_version;      /* Object file version */
  Elf64_Addr e_entry;        /* Entry point address */
  Elf64_Off e_phoff;         /* Program header offset */
  Elf64_Off e_shoff;         /* Section header offset */
  Elf64_Word e_flags;        /* Processor-specific flags */
  Elf64_Half e_ehsize;       /* ELF header size */
  Elf64_Half e_phentsize;    /* Size of program header entry */
  Elf64_Half e_phnum;        /* Number of program header entries */
  Elf64_Half e_shentsize;    /* Size of section header entry */
  Elf64_Half e_shnum;        /* Number of section header entries */
  Elf64_Half e_shstrndx;     /* Section name string table index */
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
  SHT_NULL = 0,                    /* Section header table entry unused */
  SHT_PROGBITS = 1,                /* Program data */
  SHT_SYMTAB = 2,                  /* Symbol table */
  SHT_STRTAB = 3,                  /* String table */
  SHT_RELA = 4,                    /* Relocation entries with addends */
  SHT_HASH = 5,                    /* Symbol hash table */
  SHT_DYNAMIC = 6,                 /* Dynamic linking information */
  SHT_NOTE = 7,                    /* Notes */
  SHT_NOBITS = 8,                  /* Program space with no data (bss) */
  SHT_REL = 9,                     /* Relocation entries, no addends */
  SHT_SHLIB = 10,                  /* Reserved */
  SHT_DYNSYM = 11,                 /* Dynamic linker symbol table */
  SHT_INIT_ARRAY = 14,             /* Array of constructors */
  SHT_FINI_ARRAY = 15,             /* Array of destructors */
  SHT_PREINIT_ARRAY = 16,          /* Array of pre-constructors */
  SHT_GROUP = 17,                  /* Section group */
  SHT_SYMTAB_SHNDX = 18,           /* Extended section indeces */
  SHT_NUM = 19,                    /* Number of defined types.  */
  SHT_LOOS = 0x60000000,           /* Start OS-specific.  */
  SHT_GNU_ATTRIBUTES = 0x6ffffff5, /* Object attributes.  */
  SHT_GNU_HASH = 0x6ffffff6,       /* GNU-style hash table.  */
  SHT_GNU_LIBLIST = 0x6ffffff7,    /* Prelink library list */
  SHT_CHECKSUM = 0x6ffffff8,       /* Checksum for DSO content.  */
  SHT_LOSUNW = 0x6ffffffa,         /* Sun-specific low bound.  */
  SHT_SUNW_move = 0x6ffffffa,
  SHT_SUNW_COMDAT = 0x6ffffffb,
  SHT_SUNW_syminfo = 0x6ffffffc,
  SHT_GNU_verdef = 0x6ffffffd,  /* Version definition section.  */
  SHT_GNU_verneed = 0x6ffffffe, /* Version needs section.  */
  SHT_GNU_versym = 0x6fffffff,  /* Version symbol table.  */
  SHT_HISUNW = 0x6fffffff,      /* Sun-specific high bound.  */
  SHT_HIOS = 0x6fffffff,        /* End OS-specific type */
  SHT_LOPROC = 0x70000000,      /* Start of processor-specific */
  SHT_HIPROC = 0x7fffffff,      /* End of processor-specific */
  SHT_LOUSER = 0x80000000,      /* Start of application-specific */
  SHT_HIUSER = 0x8fffffff,      /* End of application-specific */
};

enum SH_FLAG {
  SHF_WRITE = (1 << 0),            /* Writable */
  SHF_ALLOC = (1 << 1),            /* Occupies memory during execution */
  SHF_EXECINSTR = (1 << 2),        /* Executable */
  SHF_MERGE = (1 << 4),            /* Might be merged */
  SHF_STRINGS = (1 << 5),          /* Contains nul-terminated strings */
  SHF_INFO_LINK = (1 << 6),        /* `sh_info' contains SHT index */
  SHF_LINK_ORDER = (1 << 7),       /* Preserve order after combining */
  SHF_OS_NONCONFORMING = (1 << 8), /* Non-standard OS specific handling required */
  SHF_GROUP = (1 << 9),            /* Section is member of a group.  */
  SHF_TLS = (1 << 10),             /* Section hold thread-local data.  */
  SHF_COMPRESSED = (1 << 11),      /* Section with compressed data. */
  SHF_MASKOS = 0x0ff00000,         /* OS-specific.  */
  SHF_MASKPROC = 0xf0000000,       /* Processor-specific */
  SHF_ORDERED = (1 << 30),         /* Special ordering requirement (Solaris).  */
  SHF_EXCLUDE = (1U << 31),        /* Section is excluded unless referenced
                                    * or allocated (Solaris).*/
};

typedef struct {
  Elf64_Word sh_name;       /* Section name */
  Elf64_Word sh_type;       /* Section type */
  Elf64_Xword sh_flags;     /* Section attributes */
  Elf64_Addr sh_addr;       /* Virtual address in memory */
  Elf64_Off sh_offset;      /* Offset in file */
  Elf64_Xword sh_size;      /* Size of section */
  Elf64_Word sh_link;       /* Link to other section */
  Elf64_Word sh_info;       /* Miscellaneous information */
  Elf64_Xword sh_addralign; /* Address alignment boundary */
  Elf64_Xword sh_entsize;   /* Size of entries, if section has table */
} Elf64_Shdr;

#define ELF32_ST_BIND(val)        (((unsigned char) (val)) >> 4)
#define ELF32_ST_TYPE(val)        ((val) & 0xf)
#define ELF32_ST_INFO(bind, type) (((bind) << 4) + ((type) & 0xf))

#define ELF64_ST_BIND(val)        ELF32_ST_BIND(val)
#define ELF64_ST_TYPE(val)        ELF32_ST_TYPE(val)
#define ELF64_ST_INFO(bind, type) ELF32_ST_INFO((bind), (type))

enum ST_BIND {
  STB_LOCAL = 0,       /* Local symbol */
  STB_GLOBAL = 1,      /* Global symbol */
  STB_WEAK = 2,        /* Weak symbol */
  STB_NUM = 3,         /* Number of defined types.  */
  STB_LOOS = 10,       /* Start of OS-specific */
  STB_GNU_UNIQUE = 10, /* Unique symbol.  */
  STB_HIOS = 12,       /* End of OS-specific */
  STB_LOPROC = 13,     /* Start of processor-specific */
  STB_HIPROC = 15,     /* End of processor-specific */
};
enum ST_TYPE {
  STT_NOTYPE = 0,     /* Symbol type is unspecified */
  STT_OBJECT = 1,     /* Symbol is a data object */
  STT_FUNC = 2,       /* Symbol is a code object */
  STT_SECTION = 3,    /* Symbol associated with a section */
  STT_FILE = 4,       /* Symbol's name is file name */
  STT_COMMON = 5,     /* Symbol is a common data object */
  STT_TLS = 6,        /* Symbol is thread-local data object*/
  STT_NUM = 7,        /* Number of defined types.  */
  STT_LOOS = 10,      /* Start of OS-specific */
  STT_GNU_IFUNC = 10, /* Symbol is indirect code object */
  STT_HIOS = 12,      /* End of OS-specific */
  STT_LOPROC = 13,    /* Start of processor-specific */
  STT_HIPROC = 15,    /* End of processor-specific */
};

typedef struct {
  Elf64_Word st_name;     /* Symbol name */
  unsigned char st_info;  /* Type and Binding attributes */
  unsigned char st_other; /* Reserved */
  Elf64_Half st_shndx;    /* Section table index */
  Elf64_Addr st_value;    /* Symbol value */
  Elf64_Xword st_size;    /* Size of object (e.g., common) */
} Elf64_Sym;

#define ELF64_R_SYM(i)     ((i) >> 32)
#define ELF64_R_TYPE(i)    ((i) & 0xffffffffL)
#define ELF64_R_INFO(s, t) (((s) << 32) + ((t) & 0xffffffffL))

typedef struct {
  Elf64_Addr r_offset; /* Address of reference */
  Elf64_Xword r_info;  /* Symbol index and type of relocation */
} Elf64_Rel;

typedef struct {
  Elf64_Addr r_offset;   /* Address of reference */
  Elf64_Xword r_info;    /* Symbol index and type of relocation */
  Elf64_Sxword r_addend; /* Constant part of expression */
} Elf64_Rela;

enum P_TYPE {
  PT_NULL = 0,                  /* Program header table entry unused */
  PT_LOAD = 1,                  /* Loadable program segment */
  PT_DYNAMIC = 2,               /* Dynamic linking information */
  PT_INTERP = 3,                /* Program interpreter */
  PT_NOTE = 4,                  /* Auxiliary information */
  PT_SHLIB = 5,                 /* Reserved */
  PT_PHDR = 6,                  /* Entry for header table itself */
  PT_TLS = 7,                   /* Thread-local storage segment */
  PT_NUM = 8,                   /* Number of defined types */
  PT_LOOS = 0x60000000,         /* Start of OS-specific */
  PT_GNU_EH_FRAME = 0x6474e550, /* GCC .eh_frame_hdr segment */
  PT_GNU_STACK = 0x6474e551,    /* Indicates stack executability */
  PT_GNU_RELRO = 0x6474e552,    /* Read-only after relocation */
  PT_LOSUNW = 0x6ffffffa,
  PT_SUNWBSS = 0x6ffffffa,   /* Sun Specific segment */
  PT_SUNWSTACK = 0x6ffffffb, /* Stack segment */
  PT_HISUNW = 0x6fffffff,
  PT_HIOS = 0x6fffffff,   /* End of OS-specific */
  PT_LOPROC = 0x70000000, /* Start of processor-specific */
  PT_HIPROC = 0x7fffffff, /* End of processor-specific */
};

enum P_FLAG {
  PF_X = (1 << 0),          /* Segment is executable */
  PF_W = (1 << 1),          /* Segment is writable */
  PF_R = (1 << 2),          /* Segment is readable */
  PF_MASKOS = 0x0ff00000,   /* OS-specific */
  PF_MASKPROC = 0xf0000000, /* Processor-specific */
};

typedef struct {
  Elf64_Word p_type;
  Elf64_Word p_flags;
  Elf64_Off p_offset;
  Elf64_Addr p_vaddr;
  Elf64_Addr p_paddr;
  Elf64_Xword p_filesz;
  Elf64_Xword p_memsz;
  Elf64_Xword p_align;
} Elf64_Phdr;

enum D_TAG_TYPE {
  DT_NULL = 0,                 /* Marks end of dynamic section */
  DT_NEEDED = 1,               /* Name of needed library */
  DT_PLTRELSZ = 2,             /* Size in bytes of PLT relocs */
  DT_PLTGOT = 3,               /* Processor defined value */
  DT_HASH = 4,                 /* Address of symbol hash table */
  DT_STRTAB = 5,               /* Address of string table */
  DT_SYMTAB = 6,               /* Address of symbol table */
  DT_RELA = 7,                 /* Address of Rela relocs */
  DT_RELASZ = 8,               /* Total size of Rela relocs */
  DT_RELAENT = 9,              /* Size of one Rela reloc */
  DT_STRSZ = 10,               /* Size of string table */
  DT_SYMENT = 11,              /* Size of one symbol table entry */
  DT_INIT = 12,                /* Address of init function */
  DT_FINI = 13,                /* Address of termination function */
  DT_SONAME = 14,              /* Name of shared object */
  DT_RPATH = 15,               /* Library search path (deprecated) */
  DT_SYMBOLIC = 16,            /* Start symbol search here */
  DT_REL = 17,                 /* Address of Rel relocs */
  DT_RELSZ = 18,               /* Total size of Rel relocs */
  DT_RELENT = 19,              /* Size of one Rel reloc */
  DT_PLTREL = 20,              /* Type of reloc in PLT */
  DT_DEBUG = 21,               /* For debugging; unspecified */
  DT_TEXTREL = 22,             /* Reloc might modify .text */
  DT_JMPREL = 23,              /* Address of PLT relocs */
  DT_BIND_NOW = 24,            /* Process relocations of object */
  DT_INIT_ARRAY = 25,          /* Array with addresses of init fct */
  DT_FINI_ARRAY = 26,          /* Array with addresses of fini fct */
  DT_INIT_ARRAYSZ = 27,        /* Size in bytes of DT_INIT_ARRAY */
  DT_FINI_ARRAYSZ = 28,        /* Size in bytes of DT_FINI_ARRAY */
  DT_RUNPATH = 29,             /* Library search path */
  DT_FLAGS = 30,               /* Flags for the object being loaded */
  DT_ENCODING = 32,            /* Start of encoded range */
  DT_PREINIT_ARRAY = 32,       /* Array with addresses of preinit fct*/
  DT_PREINIT_ARRAYSZ = 33,     /* size in bytes of DT_PREINIT_ARRAY */
  DT_SYMTAB_SHNDX = 34,        /* Address of SYMTAB_SHNDX section */
  DT_NUM = 35,                 /* Number used */
  DT_LOOS = 0x6000000d,        /* Start of OS-specific */
  DT_HIOS = 0x6ffff000,        /* End of OS-specific */
  DT_LOPROC = 0x70000000,      /* Start of processor-specific */
  DT_HIPROC = 0x7fffffff,      /* End of processor-specific */
#define DT_PROCNUM DT_MIPS_NUM /* Most used by any processor */
};

typedef struct {
  Elf64_Sxword d_tag;
  union {
    Elf64_Xword d_val;
    Elf64_Addr d_ptr;
  } d_un;
} Elf64_Dyn;

extern Elf64_Dyn _DYNAMIC[];

#endif  // MACHINE_ELF64_H
