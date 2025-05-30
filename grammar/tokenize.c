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
 * Module Name: grammar
 * Filename: tokenize.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/
#include "tokenize.h"
#include "array.h"
#include "enum.h"
#include "generated/tokens.gen.h"
#include "string_t.h"
#include "terminal.h"

#define lenof(str_literal) ((sizeof str_literal) - 1)
#define max(a, b)          ((a) > (b) ? (a) : (b))
#define min(a, b)          ((a) < (b) ? (a) : (b))

uint32_t t_IDENTIFIER(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t t_NUMBER_adic16(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t t_NUMBER_adic10(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t t_NUMBER_adic8(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t t_NUMBER_adic2(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_instruction(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_immediate(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_machine(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_memory(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_register(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_set(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_list(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_macro(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_unsigned(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_signed(const char_t *input, Terminal *result, const Allocator *allocator);

uint32_t pass_whitespace(const char *input);

#define startswithDigital(pText) ('0' <= (pText)[0] && (pText)[0] <= '9')
#define startswithLetter(pText) \
  (('a' <= (pText)[0] && (pText)[0] <= 'z') || ('A' <= (pText)[0] && (pText)[0] <= 'Z'))

inline uint32_t t_NUMBER_adic16(
    const char_t * const input, Terminal * const result,
    const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0LL;
  while (true) {
    if ('0' <= *pText && *pText <= '9') {
      value = (value << 4) + (*pText++ - '0');
    } else if ('a' <= *pText && *pText <= 'f') {
      value = (value << 4) + (*pText++ - 'a' + 0xa);  // NOLINT(*-magic-numbers)
    } else if ('A' <= *pText && *pText <= 'F') {
      value = (value << 4) + (*pText++ - 'A' + 0xA);  // NOLINT(*-magic-numbers)
    } else if (('g' <= *pText && *pText <= 'z') || ('G' <= *pText && *pText <= 'Z')
               || ('_' == *pText)) {
      result->length = pText - input;
      return 0;
    } else if (*pText == '\'') {
      pText++;
    } else {
      break;
    }
  }
  result->type = Machine_TOKEN_NUMBER;
  result->value = (void *) value;
  result->length = pText - input;
  return result->length;
}

inline uint32_t t_NUMBER_adic10(
    const char_t * const input, Terminal * const result,
    const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if (startswithDigital(pText)) {
      value = (value * 10) + (*pText++ - '0');  // NOLINT(*-magic-numbers)
      continue;
    }
    if (startswithLetter(pText)) {
      result->length = pText - input;
      return 0;
    }
    if (*pText == '\'') {
      pText++;
    } else {
      break;
    }
  }
  result->type = Machine_TOKEN_NUMBER;
  result->value = (void *) value;
  result->length = pText - input;
  return result->length;
}

inline uint32_t t_NUMBER_adic8(
    const char_t * const input, Terminal * const result,
    const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' <= *pText && *pText <= '7') {
      value = (value << 3) + (*pText++ - '0');
      continue;
    }
    if (('8' == *pText) || ('9' == *pText) || startswithLetter(pText)) {
      result->length = pText - input;
      return 0;
    }
    if (*pText == '\'') {
      pText++;
    } else {
      break;
    }
  }
  result->type = Machine_TOKEN_NUMBER;
  result->value = (void *) value;
  result->length = pText - input;
  return result->length;
}

inline uint32_t t_NUMBER_adic2(
    const char_t * const input, Terminal * const result,
    const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' == *pText || *pText == '1') {
      value = (value << 1) + (*pText++ - '0');
      continue;
    }
    if (('2' <= *pText && *pText <= '9') || startswithLetter(pText)) {
      result->length = pText - input;
      return 0;
    }
    if (*pText == '\'') {
      pText++;
    } else {
      break;
    }
  }
  result->type = Machine_TOKEN_NUMBER;
  result->value = (void *) value;
  result->length = pText - input;
  return result->length;
}

inline uint32_t t_IDENTIFIER(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  if (startswithLetter(pText) || (*pText == '_')) {
    pText++;
  } else {
    result->length = pText - input;
    return 0;
  }
  while (true) {
    if (startswithLetter(pText) || startswithDigital(pText) || (*pText == '_')) {
      pText++;
    } else {
      break;
    }
  }
  const uint32_t len = pText - input;
  result->type = Machine_TOKEN_IDENTIFIER;
  result->value = allocator->calloc(len + 1, sizeof(char_t));
  allocator->memcpy(result->value, input, len);
  ((char_t *) result->value)[len] = '\0';
  result->length = pText - input;
  return result->length;
}

#define fn_try_keyword(_kw, _type)                                                           \
  inline uint32_t try_keyword_##_kw(                                                         \
      const char_t * const input, Terminal * const result, const Allocator * const allocator \
  ) {                                                                                        \
    const char_t pattern[] = string_t(#_kw);                                                 \
    for (uint32_t i = 2; i < sizeof(pattern) - 1; i++) {                                     \
      if (input[i - 2] != pattern[i]) { goto __failed_kw_##_kw; }                            \
    }                                                                                        \
    const char_t * const tail = &input[sizeof(pattern) - 3];                                 \
    if (startswithLetter(tail) || *tail == '_') { goto __failed_kw_##_kw; }                  \
    result->type = Machine_TOKEN_##_type;                                                    \
    result->value = nullptr;                                                                 \
    result->length = lenof(#_kw);                                                            \
    return lenof(#_kw);                                                                      \
    __failed_kw_##_kw : return t_IDENTIFIER(input - 2, result, allocator);                   \
  }
#define fn_try_keyword_val(_kw, _type, val)                                                  \
  inline uint32_t try_keyword_##_kw(                                                         \
      const char_t * const input, Terminal * const result, const Allocator * const allocator \
  ) {                                                                                        \
    const char_t pattern[] = string_t(#_kw);                                                 \
    for (uint32_t i = 2; i < sizeof(pattern) - 1; i++) {                                     \
      if (input[i - 2] != pattern[i]) { goto __failed_kw_##_kw; }                            \
    }                                                                                        \
    const char_t * const tail = &input[sizeof(pattern) - 3];                                 \
    if (startswithLetter(tail) || *tail == '_') { goto __failed_kw_##_kw; }                  \
    result->type = Machine_TOKEN_##_type;                                                    \
    result->value = (void *) val;                                                            \
    result->length = lenof(#_kw);                                                            \
    return lenof(#_kw);                                                                      \
    __failed_kw_##_kw : return t_IDENTIFIER(input - 2, result, allocator);                   \
  }

fn_try_keyword(immediate, IMMEDIATE)
fn_try_keyword(instruction, INSTRUCTION)
fn_try_keyword(machine, MACHINE)
fn_try_keyword(memory, MEMORY)
fn_try_keyword(set, SET)
fn_try_keyword(list, LIST)
fn_try_keyword(macro, MACRO)
fn_try_keyword(register, REGISTER)
fn_try_keyword_val(unsigned, TYPE, IT_UNSIGNED)
fn_try_keyword_val(signed, TYPE, IT_SIGNED)
#define fn_fall_through()                                         \
  do {                                                            \
    uint32_t length = t_IDENTIFIER(input - 1, result, allocator); \
    if (length == 0) {                                            \
      result->type = Machine_TOKEN_IDENTIFIER;                    \
      result->value = allocator->calloc(2, sizeof(char_t));       \
      allocator->memcpy(result->value, input, 1);                 \
      ((char_t *) result->value)[1] = '\0';                       \
      result->length = 1;                                         \
      return 1;                                                   \
    }                                                             \
    return length;                                                \
  } while (0)

uint32_t tokenize_prefix_in(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  if (!startswithLetter(pText)) {
    result->type = Machine_TOKEN_IN;
    result->value = nullptr;
    result->length = 2;
    return 2;
  }
  return try_keyword_instruction(input, result, allocator);
}

uint32_t tokenize_letter_i(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'm': {
      return try_keyword_immediate(input + 1, result, allocator);
    }
    case 'n': {
      return tokenize_prefix_in(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_l(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'i': {
      return try_keyword_list(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_startswith_mac(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input + 1;
  switch (*pText) {
    case 'h': {
      return try_keyword_machine(input, result, allocator);
    }
    case 'r': {
      return try_keyword_macro(input, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_m(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'a': {
      const char_t *pText = input + 1;
      if (*pText == 'c') {
        return tokenize_startswith_mac(input + 1, result, allocator);
      } else {
        fn_fall_through();
      }
    }
    case 'e': {
      return try_keyword_memory(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_s(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'e': {
      return try_keyword_set(input + 1, result, allocator);
    }
    case 'i': {
      return try_keyword_signed(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_r(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'e': {
      return try_keyword_register(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_u(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'n': {
      return try_keyword_unsigned(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_LSQUARE_startswith_digital(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  uint32_t length = t_NUMBER_adic10(input, result, allocator);
  if (length == 0) { return 0; }
  uint32_t value = (uint32_t) (uint64_t) result->value;
  const char_t *pText = input + length;
  pText += pass_whitespace(pText);
  if (*pText == ']') {
    result->type = Machine_TOKEN_WIDTH;
    result->value = (void *) (uint64_t) value;
    result->length = (pText + 1 - input);
    return result->length;
  } else if (*pText++ != '-') {
    result->length = (pText + 1 - input);
    return 0;
  }
  pText += pass_whitespace(pText);

  // parse width or bit field
  if (strcmp_o(pText, "byte") == lenof("byte")) {
    result->type = Machine_TOKEN_WIDTH;
    result->value = (void *) (uint64_t) (value << 3);
    pText += lenof("byte") + pass_whitespace(pText);
  } else if (strcmp_o(pText, "bit") == lenof("bit")) {
    result->type = Machine_TOKEN_WIDTH;
    result->value = (void *) (uint64_t) value;
    pText += lenof("bit") + pass_whitespace(pText);
  } else if ((length = t_NUMBER_adic10(pText, result, allocator)) > 0) {
    pText += length;
    BitField bitField = {.lower = (uint32_t) (uint64_t) result->value, .upper = 0};
    bitField.upper = max(value, bitField.lower);
    bitField.lower = min(value, bitField.lower);
    result->value = (void *) BitField_toUint64(&bitField);
    result->type = Machine_TOKEN_BIT_FIELD;
  }
  pText += pass_whitespace(pText);

  if (*pText != ']') {
    result->length = pText - input;
    return 0;
  } else {
    result->length = pText - input + 1;
    return result->length;
  }
}

uint32_t
    tokenize_LSQUARE_startswith_QUES_MARK(const char_t * const input, Terminal * const result, const Allocator *) {
  const char_t *pText = input + 1;
  pText += pass_whitespace(pText);
  if (*pText != ']') { return 0; }
  result->type = Machine_TOKEN_WIDTH;
  result->value = (void *) (uint64_t) -1;
  result->length = (pText + 1 - input);
  return result->length;
}

uint32_t tokenize_symbol_LPAREN(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  const uint32_t length = t_NUMBER_adic10(pText, result, allocator);
  if (length == 0) { goto __as_left_paren; }
  pText += length;
  pText += pass_whitespace(pText);
  if (*pText++ != '-') { goto __as_left_paren; }
  pText += pass_whitespace(pText);
  if (strcmp_o(pText, "tick") != lenof("tick")) { goto __as_left_paren; }
  pText += lenof("tick");
  pText += pass_whitespace(pText);
  if (*pText++ != ')') { goto __as_left_paren; }
  result->type = Machine_TOKEN_TIME_TICK;
  result->length = pText - input + 1;
  return result->length;
__as_left_paren:
  result->value = nullptr;
  result->type = Machine_TOKEN_LEFT_PAREN;
  result->length = 1;
  return 1;
}

uint32_t tokenize_symbol_LSQUARE(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  pText += pass_whitespace(pText);
  if (startswithDigital(pText)) {
    const uint32_t length = tokenize_LSQUARE_startswith_digital(pText, result, allocator);
    if (length > 0) {
      result->length += pText - input + 1;
      return result->length;
    }
  } else if (*pText == '?') {
    const uint32_t length = tokenize_LSQUARE_startswith_QUES_MARK(pText, result, allocator);
    if (length > 0) {
      result->length += pText - input + 1;
      return result->length;
    }
  }
  if (strcmp_o(pText, "...") == lenof("...")) {
    result->type = Machine_TOKEN_BIT_FIELD;
    BitField bitField = {.lower = -1, .upper = 0};
    result->value = (void *) BitField_toUint64(&bitField);
    pText += lenof("...");
    pText += pass_whitespace(pText);
    if (*pText == ']') {
      pText++;
      result->length = pText - input + 1;
      return result->length;
    }
    result->length = pText - input + 1;
    return 0;
  }
  result->type = Machine_TOKEN_LEFT_SQUARE_BRACKET;
  result->value = nullptr;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_LT(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  result->type = Machine_TOKEN_COND_BIN_OP;
  if (*pText == '=') {
    result->value = (void *) (uint64_t) CB_LE;
    result->length = 2;
    return 2;
  }
  if (*pText == '<') {
    result->type = Machine_TOKEN_ARITH_2_BIN_OP;
    result->value = (void *) (uint64_t) AB_LSH;
    result->length = 2;
    return 2;
  }
  result->value = (void *) (uint64_t) CB_LT;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_GT(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  result->type = Machine_TOKEN_COND_BIN_OP;
  if (*pText == '=') {
    result->value = (void *) (uint64_t) CB_GE;
    result->length = 2;
    return 2;
  }
  if (*pText == '<') {
    result->type = Machine_TOKEN_ARITH_2_BIN_OP;
    result->value = (void *) (uint64_t) AB_RSH;
    result->length = 2;
    return 2;
  }
  result->value = (void *) (uint64_t) CB_GT;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_EQ(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  if (*pText == '=') {
    result->type = Machine_TOKEN_COND_BIN_OP;
    result->value = (void *) (uint64_t) CB_EQ;
    result->length = 2;
    return 2;
  }
  result->type = Machine_TOKEN_ASSIGN;
  result->value = nullptr;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_OR(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  if (*pText == '|') {
    result->type = Machine_TOKEN_BOOL_OR;
    result->value = nullptr;
    result->length = 2;
    return 2;
  }
  result->type = Machine_TOKEN_ARITH_2_BIN_OP;
  result->value = (void *) (uint64_t) AB_OR;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_AND(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  if (*pText == '&') {
    result->type = Machine_TOKEN_BOOL_AND;
    result->value = nullptr;
    result->length = 2;
    return 2;
  }
  result->type = Machine_TOKEN_ARITH_2_BIN_OP;
  result->value = (void *) (uint64_t) AB_AND;
  result->length = 1;
  return 1;
}
uint32_t tokenize_symbol_NOT(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  const char_t *pText = input;
  if (*pText == '=') {
    result->type = Machine_TOKEN_COND_BIN_OP;
    result->value = (void *) (uint64_t) CB_NE;
    result->length = 2;
    return 2;
  }
  result->type = Machine_TOKEN_BOOL_NOT;
  result->value = nullptr;
  result->length = 1;
  return 1;
}

uint32_t tokenize_number(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  if ('0' == *input) {
    uint32_t length = 0;
    switch (input[1]) {
      case 'x':
      case 'X': {
        length = t_NUMBER_adic16(input + 2, result, allocator);
        result->length += 2;
        return length > 0 ? result->length : 0;
      }
      case 'o':
      case 'O': {
        length = t_NUMBER_adic8(input + 2, result, allocator);
        result->length += 2;
        return length > 0 ? result->length : 0;
      }
      case 'b':
      case 'B': {
        length = t_NUMBER_adic2(input + 2, result, allocator);
        result->length += 2;
        return length > 0 ? result->length : 0;
      }
      default: {
      }
    }
  }
  return t_NUMBER_adic10(input, result, allocator);
}

const struct {
  uint32_t t_type;
  uint32_t a_type;
} ARITH_SYM_TYPE_LITERALS[] = {
    {Machine_TOKEN_ARITH_0_BIN_OP, AB_ADD},
    {Machine_TOKEN_ARITH_0_BIN_OP, AB_SUB},
    {Machine_TOKEN_ARITH_1_BIN_OP, AB_MUL},
    {Machine_TOKEN_ARITH_1_BIN_OP, AB_DIV},
    {Machine_TOKEN_ARITH_1_BIN_OP, AB_MOD},
    {Machine_TOKEN_ARITH_2_BIN_OP, AB_XOR},
    {Machine_TOKEN_ARITH_2_SIN_OP, AS_INV},
};
uint32_t tokenize_arith_single_symbols(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  constexpr char_t ARITH_SYM_LITERALS[] = "+-*/%^~";
  uint32_t length = stridx_o(*input, ARITH_SYM_LITERALS);
  if (length < lenof(ARITH_SYM_LITERALS)) {
    result->type = ARITH_SYM_TYPE_LITERALS[length].t_type;
    result->value = (void *) (uint64_t) ARITH_SYM_TYPE_LITERALS[length].a_type;
    result->length = 1;
    return 1;
  }
  return 0;
}
constexpr uint32_t TERMINAL_TYPE_LITERALS[] = {
    Machine_TOKEN_LEFT_BRACKET,
    Machine_TOKEN_RIGHT_BRACKET,
    Machine_TOKEN_COLON,
    Machine_TOKEN_SEMICOLON,
    Machine_TOKEN_RIGHT_SQUARE_BRACKET,
    Machine_TOKEN_RIGHT_PAREN,
    Machine_TOKEN_COMMA,
    Machine_TOKEN_DOT,
    Machine_TOKEN_AT,
    Machine_TOKEN_QUESTION_MARK,
    Machine_TOKEN_OP_WIDTH,
    Machine_TOKEN_CONCAT
};
uint32_t tokenize_grammar_single_symbols(
    const char_t * const input, Terminal * const result, const Allocator * const
) {
  constexpr char_t SINGLE_LITERAL[] = "{}:;]),.@?#`";
  uint32_t length = stridx_o(*input, SINGLE_LITERAL);
  if (length < lenof(SINGLE_LITERAL)) {
    result->type = TERMINAL_TYPE_LITERALS[length];
    result->value = nullptr;
    result->length = 1;
    return 1;
  }
  return 0;
}

inline uint32_t single_tokenize(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'i': {
      return tokenize_letter_i(input + 1, result, allocator);
    }
    case 'l': {
      return tokenize_letter_l(input + 1, result, allocator);
    }
    case 'm': {
      return tokenize_letter_m(input + 1, result, allocator);
    }
    case 'r': {
      return tokenize_letter_r(input + 1, result, allocator);
    }
    case 's': {
      return tokenize_letter_s(input + 1, result, allocator);
    }
    case 'u': {
      return tokenize_letter_u(input + 1, result, allocator);
    }
    case '[': {
      return tokenize_symbol_LSQUARE(input + 1, result, allocator);
    }
    case '(': {
      return tokenize_symbol_LPAREN(input + 1, result, allocator);
    }
    case '=': {
      return tokenize_symbol_EQ(input + 1, result, allocator);
    }
    case '<': {
      return tokenize_symbol_LT(input + 1, result, allocator);
    }
    case '>': {
      return tokenize_symbol_GT(input + 1, result, allocator);
    }
    case '|': {
      return tokenize_symbol_OR(input + 1, result, allocator);
    }
    case '&': {
      return tokenize_symbol_AND(input + 1, result, allocator);
    }
    case '!': {
      return tokenize_symbol_NOT(input + 1, result, allocator);
    }
    default: {
    }
  }
  uint32_t length = 0;
  if (startswithDigital(input)) {
    length = tokenize_number(input, result, allocator);
    return length;
  }
  if (startswithLetter(input)) {
    length = t_IDENTIFIER(input, result, allocator);
    if (length > 0) { return length; }
  }
  length = tokenize_grammar_single_symbols(input, result, allocator);
  if (length > 0) { return length; }
  length = tokenize_arith_single_symbols(input, result, allocator);
  if (length > 0) { return length; }
  return 0;
}

uint32_t pass_whitespace(const char_t * const input) {
  const char_t *pText = input;
  while (*pText && stridx_o(*pText, " \t\n\f\v\r") < lenof(" \t\n\f\v\r")) { pText++; }
  return pText - input;
}
uint32_t
    try_pass_comment(const char * const input, uint32_t * const lineno, uint32_t * const column) {
  const char *pText = input + 1;
  if (*pText == '/') {
    do { pText++; } while (*pText != '\n');
  } else if (*pText == '*') {
    pText++;
    do {
      while (*pText != '*') {
        if (*pText == '\n') { (*lineno)++, *column = 1; }
        pText++;
      }
    } while (*(++pText) != '/');
    pText++;
  } else {
    return 0;
  }
  *column += pText - input;
  return pText - input;
}
uint32_t pass_space(const char * const input, uint32_t * const lineno, uint32_t * const column) {
  uint32_t l = lineno ? *lineno : 0;
  uint32_t c = column ? *column : 0;
  const char *pText = input;
  while (*pText) {
    switch (*pText) {
      case '\n': {
        l++;
        c = 1;
        break;
      }
      case '\f':
      case '\r':
      case ' ':
      case '\t': {
        c++;
        break;
      }
      case '/': {
        uint32_t passed = try_pass_comment(pText, &l, &c);
        if (passed) {
          pText += passed;
          continue;
        }
      }
      default: {
        goto __return;
      }
    }
    pText++;
  }
__return:
  lineno ? *lineno = l : 0;
  column ? *column = c : 0;
  return pText - input;
}

const Terminal *tokenize(
    const char_t * const input, uint32_t *cost, uint32_t *n_tokens, uint32_t * const lineno,
    uint32_t * const column, const Allocator * const allocator
) {  // NOLINT(*-easily-swappable-parameters)
  const char_t *pText = input;
  const uint32_t max_cost = (*cost) > 0 ? *cost : UINT32_MAX;
  *cost = 0;
  uint32_t l = lineno ? *lineno : 0;
  uint32_t c = column ? *column : 0;
  Array *terminals = Array_new(sizeof(Terminal), Machine_TOKEN_TERMINATOR, allocator);
  Terminal terminal = {};
  pText += pass_space(pText, &l, &c);
  while (*pText && pText - input < max_cost) {
    terminal.lineno = l;
    terminal.column = c;
    *cost = single_tokenize(pText, &terminal, allocator);
    c += terminal.length;
    if (0 == *cost) { break; }
    pText += *cost;
    pText += pass_space(pText, &l, &c);
    Array_append(terminals, &terminal, 1);
  }
  if ('\0' == *pText) {
    terminal.type = Machine_TOKEN_TERMINATOR;
    terminal.value = nullptr;
    terminal.lineno = l;
    terminal.column = c;
    terminal.length = 0;
    Array_append(terminals, &terminal, 1);
  }
  *cost = (uint32_t) (pText - input);
  *n_tokens = Array_length(terminals);
  const Terminal *pTerminals = (*n_tokens == 0) ? nullptr : Array_real_addr(terminals, 0);
  Array_destroy(terminals);
  lineno ? *lineno = l : 0;
  column ? *column = c : 0;
  return pTerminals;
}

inline const char_t *get_name(uint16_t type) {
  return TOKEN_NAMES[type];
}
