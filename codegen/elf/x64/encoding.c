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
 * Filename: encoding.c
 * Creator: Yaokai Liu
 * Create Date: 2025-04-07
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "encoding.h"
#include <stdio.h>
#include <string.h>
#define USING_INSTR_DIRECTLY 1
#include "call-convention.h"

void gen_elf_encoding_enter_block(
    Array *buffer, const InstrForm *form, const ParseContext *context, const AVLTree *enum_tree,
    const Array *enum_array, AVLTree *alloc_tree
);

#define ctx_ident_real(ptr) Array_virt2real(ident_array, ptr)

#define Elf64_add_text_sym(name, value, size, bind, type, visible) \
  do {                                                             \
    Elf64_Sym sym = {};                                            \
    sym.st_name = Array_length(strtab);                            \
    Array_append(strtab, name, strlen(name) + 1);                  \
    sym.st_info = ELF64_ST_INFO(bind, type);                       \
    sym.st_other = ELF64_ST_VISIBILITY(visible);                   \
    sym.st_shndx = enum_elf_text;                                  \
    sym.st_value = offset + value;                                 \
    sym.st_size = size;                                            \
    Array_append(symtab, &sym, 1);                                 \
  } while (false)

#define Elf64_add_rel(offset, sym, type)  \
  do {                                    \
    Elf64_Rel rel = {};                   \
    rel.r_offset = offset;                \
    rel.r_info = ELF64_R_INFO(sym, type); \
    Array_append(rel_ro, &rel, 1);        \
  } while (false)

void GenElf_gen_encoding(Elf64Generator *generator, const Machine *machine) {
  const ParseContext * const context = machine->context;
  const Array * const ident_array = ((Generator *) generator)->ident_array;
  const AVLTree * const enum_tree = Elf64Generator_getEnumTree(generator);
  const Array * const enum_array = Elf64Generator_getEnumArray(generator);
  Elf64Compositor *compositor = Elf64Generator_getCompositor(generator);

  Array *text_sec = Elf64_get_section(compositor, (uint8_t *) TEXT_TAG);
  Array *symtab = Elf64_get_section(compositor, (uint8_t *) SYMTAB_TAG);
  Array *strtab = Elf64_get_section(compositor, (uint8_t *) STRTAB_TAG);

  char_t name_buffer[256] = {};
  x64Machine *x64_machine = x64Machine_new(compositor->allocator);
  useMachine(x64_machine);
  AVLTree *alloc_tree = AVLTree_new(compositor->allocator, nullptr);
  const uint32_t n_instrs = Array_length(context->instrArray);
  const Instruction * const instrs = Array_first_real(context->instrArray);
  for (uint32_t i = 0; i < n_instrs; i++) {
    const uint32_t offset = Elf64_curr_offset(compositor, (uint8_t *) TEXT_TAG);
    const char_t * const name = Array_virt2real(ident_array, instrs[i].name);
    const uint32_t n_forms = Array_length(instrs[i].forms);
    const InstrForm *forms = Array_first_real(instrs[i].forms);
    for (uint32_t j = 0; j < n_forms; j++) {
      gen_elf_encoding_enter_block(text_sec, &forms[i], context, enum_tree, enum_array, alloc_tree);
    }
    const uint32_t size = Elf64_curr_offset(compositor, (uint8_t *) TEXT_TAG) - offset;
    sprintf(name_buffer, "encoding_%s_%u", name, i);
    Elf64_add_text_sym(name_buffer, 0, size, STB_LOCAL, STT_FUNC, STV_HIDDEN);
  }
  x64Machine_destroy(x64_machine);
}
struct spill_record {
  uint32_t offset;
  uint32_t need_rescure;
  const Entry * const *p_entry;
};
const Entry *const *REX_REGS[8] = {
    &REG_r8, &REG_r9, &REG_r10, &REG_r11,
    &REG_r12, &REG_r13, &REG_r14, &REG_r15
};
#define min(a, b)   ((a) < (b) ? (a) : (b))
#define align_to(a, m)  ((a) ? (((a) - 1) / (m) + 1) * (m) : 0)

void gen_elf_encoding_enter_block(
    Array *buffer, const InstrForm *form, const ParseContext *, const AVLTree *, const Array *,
    AVLTree *alloc_tree
) {
#define PROG_INSTR_BUFFER buffer
  push(REG_rbp);
  mov(REG_rbp, REG_rsp);
  push(REG_rdi);
  const uint32_t n_arg_in_reg = min(8, form->pattern->used_args);
  for (uint32_t i = 4; i < n_arg_in_reg; i ++) {
    if (AVLTree_get(alloc_tree, (uint64_t) *REX_REGS[i])) {
      AVLTree_set(alloc_tree, (uint64_t) *REX_REGS[i], (void *) *REX_REGS[i]);
      push(*REX_REGS[i]);
    }
  }

  for (uint32_t i = 0; i < n_arg_in_reg; i ++) {
    mov(*(REX_REGS[i]), MEM_REFv(REG_rsi)); // TODO: using SIB instead of REF
  }
  for (uint32_t i = n_arg_in_reg; i < form->pattern->used_args; i ++) {
    push(MEM_REFv(REG_rsi));  // TODO: using SIB instead of REF
  }
  subi(REG_rsp, IMM_Ir(align_to(form->width, 16)));

  for (uint32_t i = n_arg_in_reg - 1; i >= 4; i --) {
    pop(*(REX_REGS[i]));
  }
  mov(REG_rsp, REG_rbp);
  pop(REG_rbp);
#undef PROG_INSTR_BUFFER
}

