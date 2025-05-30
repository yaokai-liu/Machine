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
 * Module Name: codegen
 * Filename: generate.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "generate.h"
#include "define.h"
#include "encoding.h"
#include "generator.h"
#include "parse/target.h"
#include "pattern_match.h"
#include "static.h"
#include <stdint.h>

int32_t codegen(Generator *generator, const Machine *machine) {
  gen_export_header(generator, machine);
  gen_export_record_declare(generator, machine);
  gen_export_instr_macro(generator, machine);
  gen_export_tail(generator, machine);

  switch (generator->gen_type) {
    case GT_C: {
      GenC_gen_static_definitions(generator, machine);
      GenC_gen_context_dec(generator, machine);
      GenC_gen_context_def(generator, machine);
      GenC_gen_pattern_match(generator, machine);
      GenC_gen_enum_item(generator, machine);
      GenC_gen_instr_exec(generator, machine);
      GenC_gen_set_grp_jump_table(generator, machine);
      //    GenC_gen_reg_grp_table(generator, machine);
      GenC_gen_driver(generator, machine);
      break;
    }
  }
  return 0;
}
