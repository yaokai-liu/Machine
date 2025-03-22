/**
 * License
 *
 * xMachine - A Backend Generator for Compilers
 * Copyright (C) 2024 Yaokai Liu
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
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: static.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "static.h"
#include "gen-export.h"
#include "generate.h"
#include "generator.h"
#include <stdio.h>
#include <string.h>

constexpr char_t INCLUDES[] = "#include \"%s.h\"\n"
                              "#include <stdarg.h>\n";

constexpr char_t MACROS[] =
    "#define max(a, b)             ((a) > (b)) ? (a) : (b)\n"
    "#define UINT_N_MAX(n_bits)    (n_bits == 64 ? (-1LLU) : ((1LLU << (n_bits)) - 1))\n"
    "#define LOW_BITS(val, n_bits) ((val) & UINT_N_MAX(n_bits))\n"
    "#define MASK_BITS(bl, bu)     (UINT_N_MAX(bu) - UINT_N_MAX(bl))\n"
    "#define numSetBits(num, bl, bu, val) \\\n"
    "  (((num) & ~MASK_BITS(bl, bu)) | (LOW_BITS(val, (bu) - (bl)) << (bl)))\n"
    "#define setEncodingNumber(val) \\\n"
    "  do { value = (val); } while (false)\n"
    "#define pushInstrBytes(_size)                  \\\n"
    "  do {                                         \\\n"
    "    uint32_t last = max(_size + index, size);  \\\n"
    "    for (uint32_t i = index; i < last; i++) {  \\\n"
    "      bytes[i] = value & 0xFF;                 \\\n"
    "      value >>= 8;                             \\\n"
    "    }                                          \\\n"
    "    index = last;                              \\\n"
    "  } while (false)\n"
    "#define pushEncodingNumber(val, size) \\\n"
    "  do {                                \\\n"
    "    setEncodingNumber(val);           \\\n"
    "    pushInstrBytes(size);             \\\n"
    "  } while (false)\n";

constexpr char_t TYPEDEF_ENTRY_FMT[] = "typedef struct Entry {\n"
                                       "  enum ENTRY_TYPE_ENUM type;\n"
                                       "  uint32_t width;\n"
                                       "  uint64_t value;\n"
                                       "  enum ENTRY_TYPE_ENUM subtypes[%u];\n"
                                       "} Entry;\n";

constexpr char_t TYPEDEF_MACHINE_FMT[] = "typedef struct Machine {\n"
                                         "  const Allocator *allocator;"
                                         "  uint32_t argCount;\n"
                                         "  Entry entries[%u];\n"
                                         "} Machine;\n";

constexpr char_t CURRENT_MACHINE[] = "Machine *CURRENT_MACHINE;\n";

constexpr char_t MAX_ARGS_DECLARE[] = "const uint32_t MAX_ARGS;\n";
constexpr char_t MAX_ARGS_FMT[] = "const uint32_t MAX_ARGS = %u;\n";

constexpr char_t STRUCT_JUMP_ITEM[] = "struct jump_item {\n"
                                      "  enum ENTRY_TYPE_ENUM expected_type;\n"
                                      "  uint32_t next_state_index;\n"
                                      "};\n";

constexpr char_t STRUCT_JUMP_STATE[] = "struct jump_state {\n"
                                       "  uint32_t count;\n"
                                       "  uint32_t index;\n"
                                       "  uint32_t (*fn_encoding)(Array *, const Entry *[]);\n"
                                       "};\n";

constexpr char_t STRUCT_SET_GRP_JUMP_STATE[] = "struct set_grp_jump_state {\n"
                                               "  uint32_t count;\n"
                                               "  uint32_t index;\n"
                                               "};\n";

constexpr char_t ENTRY_TYPE_CHECK_DEC[] =
    "bool entry_type_check(enum ENTRY_TYPE_ENUM type1, enum ENTRY_TYPE_ENUM type2);\n";
constexpr char_t ENTRY_TYPE_CHECK_DEF[] =
    "bool entry_type_check(enum ENTRY_TYPE_ENUM type1, enum ENTRY_TYPE_ENUM type2) {\n"
    "  if (type1 == type2) { return type1 < enum_BEGIN_SET_GRP; }\n"
    "  else if (enum_BEGIN_SET_GRP < type1 && type1 < enum_TYPE_ENUM_UPPER_BOUND) {\n"
    "    const struct set_grp_jump_state * state = &SET_GRP_STATE_TABLE[type1 - enum_BEGIN_SET_GRP "
    "- 1];\n"
    "    for (uint32_t i = 0 ; i < state->count; i ++) {\n"
    "      type1 = SET_GRP_VAL_TABLE[state->index + i];\n"
    "      if (entry_type_check(type1, type2)) { return true; }\n"
    "    }\n"
    "  }\n"
    "  return false;\n"
    "}\n";
constexpr char_t MACHINE_NEW_DEF[] = "inline Machine *Machine_new(const Allocator *allocator) {\n"
                                     "  Machine *machine = allocator->calloc(1, sizeof(Machine));\n"
                                     "  machine->allocator = allocator;\n"
                                     "  return machine;\n"
                                     "}\n";
constexpr char_t MACHINE_DESTROY_DEF[] =
    "inline void Machine_destroy(Machine *machine) {\n"
    "  if (CURRENT_MACHINE == machine) { CURRENT_MACHINE = nullptr; }\n"
    "  machine->allocator->free(machine);\n"
    "}\n";
constexpr char_t USE_MACHINE_DEF[] = "inline void useMachine(Machine *machine) {\n"
                                     "  CURRENT_MACHINE = machine;\n"
                                     "}\n";

constexpr char_t CONVERT_INSTR_TO_BYTES_DEC[] =
    "uint32_t convert_instr_to_bytes(\n"
    "    uint32_t offset, Array *buffer, const Entry *entries[], uint32_t n_args\n"
    ");\n";
constexpr char_t CONVERT_INSTR_TO_BYTES_DEF[] =
    "uint32_t convert_instr_to_bytes(\n"
    "    uint32_t offset, Array *buffer, const Entry *entries[], uint32_t n_args\n"
    ") {\n"
    "  const struct jump_state *state = &JUMP_STATE_TABLE[offset];\n"
    "  uint32_t ndx = 0;\n"
    "  while (ndx < n_args) {\n"
    "    bool matched = false;\n"
    "    for (uint32_t j = 0; j < state->count; j++) {\n"
    "      const struct jump_item *type1 = &JUMP_KEY_TABLE[state->index + j];\n"
    "      if (entry_type_check(type1->expected_type, entries[ndx]->type)) {\n"
    "        matched = true;\n"
    "        offset = type1->next_state_index;\n"
    "        break;\n"
    "      }\n"
    "    }\n"
    "    if (!matched) { return 0; }\n"
    "    state = &JUMP_STATE_TABLE[offset];\n"
    "    ndx++;\n"
    "  }\n"
    "  if (!state->fn_encoding) { return 0; }\n"
    "  return state->fn_encoding(buffer, entries);\n"
    "}\n";

#define ctx_push_string(type, s)                                                   \
  do {                                                                             \
    Array_append(Generator_getOutputBuffer(generator, GenC_##type), s, strlen(s)); \
  } while (false)

#define push_string(s) \
  do { Array_append(buffer, s, strlen(s)); } while (false)

void gen_static_definitions(Generator *generator, const Machine *machine) {
  char_t temp_buffer[256];
  const char_t *name = generator->outname;
  if (name) {
    name = strrchr(name, '/');
    name = name ? name + 1 : generator->outname;
  } else {
    name = Array_virt2real(generator->ident_array, machine->name);
  }
  sprintf(temp_buffer, INCLUDES, name);
  Array * const out_buffer = Generator_getOutputBuffer(generator, GenC_includes);
  const char_t *filename = "";
  if (generator->headpath) {
    filename = strrchr(generator->headpath, '/');
    filename = filename ? filename + 1 : generator->headpath;
  }
  gen_license(generator, out_buffer, filename);
  ctx_push_string(includes, temp_buffer);
  ctx_push_string(macros, MACROS);
  sprintf(temp_buffer, TYPEDEF_ENTRY_FMT, machine->context->maxFieldCount);
  ctx_push_string(types, temp_buffer);
  sprintf(temp_buffer, TYPEDEF_MACHINE_FMT, machine->context->maxArgCount);
  ctx_push_string(types, temp_buffer);
  ctx_push_string(types, STRUCT_JUMP_ITEM);
  ctx_push_string(types, STRUCT_JUMP_STATE);
  ctx_push_string(types, STRUCT_SET_GRP_JUMP_STATE);
  ctx_push_string(declares, CURRENT_MACHINE);
  ctx_push_string(declares, MAX_ARGS_DECLARE);
  ctx_push_string(declares, ENTRY_TYPE_CHECK_DEC);
  ctx_push_string(declares, CONVERT_INSTR_TO_BYTES_DEC);
}
void gen_driver(Generator *generator, const Machine *machine) {
  char_t temp_buffer[256];
  sprintf(temp_buffer, MAX_ARGS_FMT, machine->context->maxArgCount);
  ctx_push_string(definitions, temp_buffer);
  ctx_push_string(definitions, MACHINE_NEW_DEF);
  ctx_push_string(definitions, MACHINE_DESTROY_DEF);
  ctx_push_string(definitions, USE_MACHINE_DEF);
  ctx_push_string(definitions, ENTRY_TYPE_CHECK_DEF);
  ctx_push_string(definitions, CONVERT_INSTR_TO_BYTES_DEF);
}
