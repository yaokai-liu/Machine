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
 * Module Name: grammar
 * Filename: err.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-13
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "err.h"
#include "enum.h"
#include "generated/machine/action-table.gen.h"
#include "generated/macro/action-table.gen.h"
#include <stdio.h>
#include <string.h>

char_t *COMPIE_SECTION[] = {
    [COMPILER_LEX] = "lexing",
    [COMPILER_MACRO] = "executing macro",
    [COMPILER_PARSE] = "parsing",
};

char_t *ERR_MSG[] = {
    [SUCCESS] = "success",
    [ERROR_BAD_OPERAND] =
        "arithmetic operation with 'Memory' or 'Immediate' entity is not supported",
    [END_OF_MACRO_FRAME] = "end of macro frame",
    [ERROR_UNKNOWN_WIDTH] = "can't compute width of the instruction part",
    [ERROR_NON_SET_RECORD] = "can't cant convert a non-iterable type to set",
    [ERROR_WIDTH_MISMATCH] = "width mismatch",
    [ERROR_UNKNOWN_SYMBOL] = "unknown symbol",
    [ERROR_WIDTH_TOO_LONG] = "too long to support this width",
    [ERROR_EMPTY_INSTR_PART] = "empty instruction part",
    [ERROR_MISALIGNED_WIDTH] = "with misaligned",
    [ERROR_UNEXPECTED_TOKEN] = "unexpected token",
    [ERROR_FAILED_TO_PRODUCE] = "failed to produce",
    [ERROR_CONFLICT_BIT_FIELD] = "conflict bit field",
    [ERROR_INCOMPATIBLE_WIDTH] = "incompatible width",
    [ERROR_UNDEFINED_VARIABLE] = "undefined variable",
    [ERROR_NON_ACCESSIBLE_VAR] = "non-accessible variable",
    [ERROR_INDEX_OUT_OF_RANGE] = "index out of range",
    [ERROR_MULTIPLE_DEFINE_FORM] = "duplicated instruction form",
    [ERROR_EMPTY_REGISTER_GROUP] = "empty register group",
    [ERROR_UNDEFINED_IDENTIFIER] = "undefined identifier",
    [ERROR_OPERATION_UNSUPPORTED] = "operation is not supported",
    [ERROR_MULTIPLE_DEFINE_MACRO] = "multiple define macro",
    [ERROR_REDEFINED_DEFAULT_BITS] = "redefined default bits",
    [ERROR_MULTIPLE_DEFINE_ENTRY] = "multiple define entry",
    [ERROR_ARGUMENT_COUNT_MISMATCH] = "argument count mismatch",
    [ERROR_UNEXPECTED_CONCAT_TOKEN] = "unexpected concat token",
    [ERROR_FAILED_TO_GET_NEXT_STATE] = "failed to get next state",
    [ERROR_WHATEVER] = "error whatever",
};

void get_error_msg(const ErrInfo * const errInfo, char_t *buffer) {
  if (errInfo->code == ERROR_UNEXPECTED_TOKEN) {
    sprintf(
        buffer, "when %s, %s: %s\n", COMPIE_SECTION[errInfo->stage], ERR_MSG[errInfo->code],
        TOKEN_NAMES[errInfo->token]
    );
    char_t *p_str = buffer + strlen(buffer);
    strcpy(p_str, "Expected tokens: \n");
    p_str += sizeof("Expected tokens: \n") - 1;
    uint32_t tokens[MAX_TOTAL_TOKEN] = {};
    uint32_t n_tokens = errInfo->stage == COMPILER_MACRO ?
                            getMacroStateExpectedTokenType(errInfo->state, tokens) :
                            getParseStateExpectedTokenType(errInfo->state, tokens);
    for (uint32_t i = 0; i < n_tokens; i++) {
      strcpy(p_str, TOKEN_NAMES[tokens[i]]);
      p_str += strlen(TOKEN_NAMES[tokens[i]]);
      *p_str++ = '\n';
    }
  } else {
    sprintf(buffer, "when %s, %s.\n", COMPIE_SECTION[errInfo->stage], ERR_MSG[errInfo->code]);
  }
}
