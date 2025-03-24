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
 * Filename: define.c
 * Creator: Yaokai Liu
 * Create Date: 2025-02-20
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "define.h"

#define GenElf_gen_type_enum_item(Type, var)                                          \
  do {                                                                                \
    const uint32_t count = Array_length(context->var##Array);                         \
    const Type *entries = Array_real_addr(context->var##Array, 0);                    \
    for (uint32_t i = 0; i < count; i++, offset++) {                                  \
      AVLTree_set(enum_tree, (uint64_t) entries[i].name, (void *) (uint64_t) offset); \
    }                                                                                 \
  } while (false)

void GenElf_gen_enum_item(Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  AVLTree * const enum_tree = generator->enum_tree;
  uint32_t offset = 1;
  GenElf_gen_type_enum_item(Memory, mem);
  GenElf_gen_type_enum_item(Immediate, imm);
  GenElf_gen_type_enum_item(Register, reg);
  generator->count_of_single_entries = ++offset;
  GenElf_gen_type_enum_item(RegisterGroup, grp);
  GenElf_gen_type_enum_item(EntrySet, set);
  generator->count_of_total_entries = offset;
}
typedef struct RegEntry {
  uint32_t type;
  uint32_t width;
  uint64_t value;
} RegEntry;
void GenElf_gen_reg_def(Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  AVLTree * const enum_tree = generator->enum_tree;
  Array *const rodata_array = generator->buffers[GenElf_rodata];

  const Register *regs = Array_first_real(context->regArray);
  const uint32_t n_regs = Array_length(context->regArray);

  RegEntry entry = {};
  for (uint32_t i = 0; i < n_regs; i++) {
    entry.type = (uint64_t) AVLTree_get(enum_tree, (uint64_t)regs[i].name);
    entry.width = regs[i].field.upper - regs[i].field.lower + 1;
    entry.value = regs[i].code;
    Array_append(rodata_array, &entry, 1);
  }
}

void GenElf_gen_mem_def(Generator *generator, const Array *ident_array) {
  Array *const text_array = generator->buffers[GenElf_text];

}