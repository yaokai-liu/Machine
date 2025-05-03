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
 * Filename: cache_enum.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-07
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "cache_enum.h"

#define GenElf_gen_type_enum_item(Type, var)                                          \
  do {                                                                                \
    const uint32_t count = Array_length(context->var##Array);                         \
    const Type *entries = Array_real_addr(context->var##Array, 0);                    \
    for (uint32_t i = 0; i < count; i++, offset++) {                                  \
      AVLTree_set(enum_tree, (uint64_t) entries[i].name, (void *) (uint64_t) offset); \
    }                                                                                 \
  } while (false)

void GenElf_cache_enum_item(Elf64Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  AVLTree * const enum_tree = Elf64Generator_getEnumTree(generator);
  uint32_t offset = 1;
  GenElf_gen_type_enum_item(Memory, mem);
  GenElf_gen_type_enum_item(Immediate, imm);
  GenElf_gen_type_enum_item(Register, reg);
  Elf64Generator_set_cose(generator, ++offset);
  GenElf_gen_type_enum_item(RegisterGroup, grp);
  GenElf_gen_type_enum_item(RecordSet, set);
  Elf64Generator_set_cote(generator, offset);
}
