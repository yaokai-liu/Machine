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
 * Module Name: codegen
 * Filename: export-fmt.h
 * Creator: Yaokai Liu
 * Create Date: 2025-03-24
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#ifndef MACHINE_DECLARE_FMT_H
#define MACHINE_DECLARE_FMT_H

constexpr char_t MEM_DEC_NAME_FMT[] = "const Entry *MEM_%s";
constexpr char_t IMM_DEC_FMT[] = "const Entry *IMM_%s(uint64_t val);\n";
constexpr char_t REG_DEC_FMT[] = "extern const Entry *const REG_%s;\n";

#endif  // MACHINE_DECLARE_FMT_H
