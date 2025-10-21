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
 * Module Name: codegen
 * Filename: gen-export.h
 * Creator: Yaokai Liu
 * Create Date: 2025-02-20
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_GEN_EXPORT_H
#define MACHINE_GEN_EXPORT_H

#include "generate.h"
#include <stdio.h>

void gen_license(
    const char_t *cr_holder, const char_t *year, const char_t *filename, FILE *ostream
);
void gen_export_header(const Generator *generator, const Machine *machine);
void gen_export_instr_macro(const Generator *generator, const Machine *machine);
void gen_export_record_declare(Generator *generator, const Machine *machine);
void gen_export_tail(const Generator *generator, const Machine *machine);

#endif  // MACHINE_GEN_EXPORT_H
