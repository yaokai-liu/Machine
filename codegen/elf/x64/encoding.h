/* License
 *
 * ${PROJ_DESCRIPTION}
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
 * Filename: encoding.h
 * Creator: Yaokai Liu
 * Create Date: 2025-04-07
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_ENCODING_H
#define MACHINE_ENCODING_H

#include "generator.h"
#include "parse/context.h"
void GenElf_gen_encoding(Elf64Generator *generator, const Machine *machine);

#endif  // MACHINE_ENCODING_H
