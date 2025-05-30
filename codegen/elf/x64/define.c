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
#include "trie-dump.h"
#include <compositor.h>
#include <elf.h>
#include <stdio.h>
#include <string.h>

#define ctx_ident_real(ptr) Array_virt2real(ident_array, ptr)

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

typedef struct RegEntry {
  uint32_t type;
  uint32_t width;
  uint64_t value;
} RegEntry;

#define Elf64_add_ro_sym(name, value, size, bind, type, visible) \
  do {                                                           \
    Elf64_Sym sym = {};                                          \
    sym.st_name = Array_length(strtab);                          \
    Array_append(strtab, name, strlen(name) + 1);                \
    sym.st_info = ELF64_ST_INFO(bind, type);                     \
    sym.st_other = ELF64_ST_VISIBILITY(visible);                 \
    sym.st_shndx = enum_elf_rodata;                              \
    sym.st_value = offset + value;                               \
    sym.st_size = size;                                          \
    Array_append(symtab, &sym, 1);                               \
  } while (false)

#define Elf64_add_rel(offset, sym, type)  \
  do {                                    \
    Elf64_Rel rel = {};                   \
    rel.r_offset = offset;                \
    rel.r_info = ELF64_R_INFO(sym, type); \
    Array_append(rel_ro, &rel, 1);        \
  } while (false)

void GenElf_gen_reg_def(Elf64Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  const Array * const ident_array = ((Generator *) generator)->ident_array;
  AVLTree * const enum_tree = Elf64Generator_getEnumTree(generator);
  Elf64Compositor *compositor = Elf64Generator_getCompositor(generator);

  Array *rodata = Elf64_get_section(compositor, (uint8_t *) RODATA_TAG);
  Array *symtab = Elf64_get_section(compositor, (uint8_t *) SYMTAB_TAG);
  Array *strtab = Elf64_get_section(compositor, (uint8_t *) STRTAB_TAG);
  Array *rel_ro = Elf64_get_section(compositor, (uint8_t *) REL_RODATA_TAG);

  const Register *regs = Array_first_real(context->regArray);
  const uint32_t n_regs = Array_length(context->regArray);

  const uint64_t pointer_rel = 0;
  char_t name_buffer[256] = {};
  RegEntry entry = {0, 0, 0};
  const uint32_t start_of_const_entries = Array_length(symtab);

  uint32_t offset = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG);
  Elf64_add_ro_sym("Entry_EOI", 0, sizeof(RegEntry), STB_LOCAL, STT_OBJECT, STV_HIDDEN);
  Array_append(rodata, (uint8_t *) &entry, sizeof(RegEntry));
  for (uint32_t i = 0; i < n_regs; i++) {
    entry.type = (uint64_t) AVLTree_get(enum_tree, (uint64_t) regs[i].name);
    entry.width = regs[i].field.upper - regs[i].field.lower + 1;
    entry.value = regs[i].code;
    char_t *name = ctx_ident_real(regs[i].name);
    sprintf(name_buffer, "Entry_REG_%s", name);
    Elf64_add_ro_sym(
        name_buffer, i * sizeof(RegEntry), sizeof(RegEntry), STB_LOCAL, STT_OBJECT, STV_HIDDEN
    );
    Array_append(rodata, (uint8_t *) &entry, sizeof(RegEntry));
  }

  offset = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG);
  Elf64_add_rel(offset, start_of_const_entries, R_X86_64_64);
  Elf64_add_ro_sym("EOI", 0, sizeof(uint64_t), STB_GLOBAL, STT_OBJECT, STV_DEFAULT);
  Array_append(rodata, (uint8_t *) &pointer_rel, sizeof(pointer_rel));
  for (uint32_t i = 0; i < n_regs; i++) {
    char_t *name = Array_virt2real(((Generator *) generator)->ident_array, regs[i].name);
    sprintf(name_buffer, "REG_%s", name);
    Elf64_add_rel(offset + i * sizeof(uint64_t), start_of_const_entries + i, R_X86_64_64);
    Elf64_add_ro_sym(
        name_buffer, i * sizeof(uint64_t), sizeof(uint64_t), STB_LOCAL, STT_OBJECT, STV_DEFAULT
    );
    Array_append(rodata, &pointer_rel, sizeof(pointer_rel));
  }
}
#define val_case_item(Type, var)                                            \
  case Machine_TOKEN_##Type: {                                              \
    const Type *var = Array_real_addr(context->var##Array, record->offset); \
    name = var->name;                                                       \
    break;                                                                  \
  }
void GenElf_gen_jump_table(Elf64Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  AVLTree * const enum_tree = Elf64Generator_getEnumTree(generator);
  Elf64Compositor *compositor = Elf64Generator_getCompositor(generator);

  Array *rodata = Elf64_get_section(compositor, (uint8_t *) RODATA_TAG);
  Array *symtab = Elf64_get_section(compositor, (uint8_t *) SYMTAB_TAG);
  Array *strtab = Elf64_get_section(compositor, (uint8_t *) STRTAB_TAG);
  Array *rel_ro = Elf64_get_section(compositor, (uint8_t *) REL_RODATA_TAG);

  Array *type_array = Array_new(sizeof(uint32_t), -1, compositor->allocator);
  const uint32_t n_keys = Array_length(context->keyArray);
  const TrieKeyItem *keys = Array_first_real(context->keyArray);
  for (uint32_t i = 0; i < n_keys; i++) {
    Identifier *name = nullptr;
    const Record *record = Array_virt2real(context->recordArray, (REFER(Record)) keys[i].key);
    switch (record->typeid) {
      val_case_item(Memory, mem)
      val_case_item(Immediate, imm)
      val_case_item(Register, reg)
      val_case_item(RegisterGroup, grp)
      val_case_item(RecordSet, set)
      default: {
      }
    }
    uint32_t val = (uint64_t) AVLTree_get(enum_tree, (uint64_t) name);
    Array_append(type_array, &val, 1);
  }

  const uint64_t pointer_rel = 0;
  uint32_t offset = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG);
  const uint32_t *types = Array_first_real(type_array);
  for (uint32_t i = 0; i < n_keys; i++) {
    struct {
      uint32_t type;
      uint32_t index;
    } jump_key = {};
    jump_key.type = types[i];
    jump_key.index = keys[i].next_node;
    Array_append(rodata, &jump_key, sizeof(jump_key));
  }
  releasePrimeArray(type_array);
  uint32_t tabsize = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG) - offset;
  Elf64_add_ro_sym("JUMP_KEY_TABLE", 0, tabsize, STB_LOCAL, STT_OBJECT, STV_HIDDEN);
  Array_append(rodata, &pointer_rel, sizeof(pointer_rel));

  const uint32_t start_of_jump_state_table = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG);
  const uint32_t n_states = Array_length(context->stateArray);
  const TrieNodeItem *states = Array_first_real(context->stateArray);
  for (uint32_t i = 0; i < n_states; i++) {
    struct {
      uint32_t count;
      uint32_t index;
      uint64_t fn_ptr;
    } jump_state = {};
    jump_state.count = states[i].count;
    jump_state.index = states[i].offset;
    jump_state.fn_ptr = 0;
    uint32_t sym_index = Elf64Generator_get_sym_index(generator, (uint64_t) states[i].value);
    if (sym_index) {
      offset = start_of_jump_state_table + i * sizeof(jump_state);
      Elf64_add_rel(offset + offsetof(typeof(jump_state), fn_ptr), sym_index, R_X86_64_64);
    }
    Array_append(rodata, &jump_state, sizeof(jump_state));
  }
  offset = start_of_jump_state_table;
  tabsize = Elf64_curr_offset(compositor, (uint8_t *) RODATA_TAG) - offset;
  Elf64_add_ro_sym("JUMP_STATE_TABLE", 0, tabsize, STB_LOCAL, STT_OBJECT, STV_HIDDEN);
  Array_append(rodata, &pointer_rel, sizeof(pointer_rel));
}
