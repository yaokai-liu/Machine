/**
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
#include "string_t.h"
#include "terminal.h"
#include "tokens.gen.h"

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
uint32_t try_keyword_unsigned(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t try_keyword_signed(const char_t *input, Terminal *result, const Allocator *allocator);
uint32_t single_tokenize(const char_t *input, Terminal *result, const Allocator *allocator);

uint32_t pass_space(const char_t * const input);

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
      return 0;
    } else {
      break;
    }
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

inline uint32_t t_NUMBER_adic10(
    const char_t * const input, Terminal * const result,
    const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' <= *pText && *pText <= '9') {
      value = (value * 10) + (*pText++ - '0');  // NOLINT(*-magic-numbers)
      continue;
    }
    if (('a' <= *pText && *pText <= 'z') || ('A' <= *pText && *pText <= 'Z') || ('_' == *pText)) {
      return 0;
    }
    break;
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
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
    if (('8' == *pText) || ('9' == *pText) || ('a' <= *pText && *pText <= 'z')
        || ('A' <= *pText && *pText <= 'Z') || ('_' == *pText)) {
      return 0;
    }
    break;
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
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
    if (('2' <= *pText && *pText <= '9') || ('a' <= *pText && *pText <= 'z')
        || ('A' <= *pText && *pText <= 'Z') || ('_' == *pText)) {
      return 0;
    }
    break;
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

inline uint32_t t_IDENTIFIER(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  if (('a' <= *pText && *pText <= 'z') || ('A' <= *pText && *pText <= 'Z')) {
    pText++;
  } else {
    return 0;
  }
  while (true) {
    if (('a' <= *pText && *pText <= 'z') || ('A' <= *pText && *pText <= 'Z')
        || ('0' <= *pText && *pText <= '9')) {
      pText++;
    } else {
      break;
    }
  }
  Identifier *ident = allocator->calloc(1, sizeof(Identifier));
  ident->len = pText - input;
  ident->ptr = allocator->calloc(ident->len + 1, sizeof(char_t));
  allocator->memcpy(ident->ptr, input, ident->len);
  ident->ptr[ident->len] = '\0';
  result->type = enum_IDENTIFIER;
  result->value = ident;
  return ident->len;
}

#define fn_try_keyword(_kw, _type)                                                           \
  inline uint32_t try_keyword_##_kw(                                                         \
      const char_t * const input, Terminal * const result, const Allocator * const allocator \
  ) {                                                                                        \
    const char_t pattern[] = string_t(#_kw);                                                 \
    for (uint32_t i = 2; i < sizeof(pattern) - 1; i++) {                                     \
      if (input[i - 2] != pattern[i]) { goto __failed_kw_##_kw; }                            \
    }                                                                                        \
    result->type = enum_##_type;                                                             \
    result->value = nullptr;                                                                 \
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
    result->type = enum_##_type;                                                             \
    result->value = (void *) val;                                                            \
    return lenof(#_kw);                                                                      \
    __failed_kw_##_kw : return t_IDENTIFIER(input - 2, result, allocator);                   \
  }

fn_try_keyword(immediate, IMMEDIATE)
fn_try_keyword(instruction, INSTRUCTION)
fn_try_keyword(machine, MACHINE)
fn_try_keyword(memory, MEMORY)
fn_try_keyword(set, SET)
fn_try_keyword(register, REGISTER)
fn_try_keyword_val(unsigned, TYPE, IT_UNSIGNED)
fn_try_keyword_val(signed, TYPE, IT_SIGNED)
#define fn_fall_through()                                           \
  do {                                                              \
    uint32_t length = t_IDENTIFIER(input - 1, result, allocator);   \
    if (length == 0) {                                              \
      Identifier *ident = allocator->calloc(1, sizeof(Identifier)); \
      ident->len = 1;                                               \
      ident->ptr = allocator->calloc(2, sizeof(char_t));            \
      allocator->memcpy(ident->ptr, input, 1);                      \
      ident->ptr[1] = '\0';                                         \
      result->type = enum_IDENTIFIER;                               \
      result->value = ident;                                        \
      return 1;                                                     \
    }                                                               \
    return length;                                                  \
  } while (0)

uint32_t tokenize_letter_i(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'm': {
      return try_keyword_immediate(input + 1, result, allocator);
    }
    case 'n': {
      return try_keyword_instruction(input + 1, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_m(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'a': {
      return try_keyword_machine(input + 1, result, allocator);
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

uint32_t tokenize_startswith_number(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  uint32_t length = t_NUMBER_adic10(input, result, allocator);
  uint32_t value = (uint32_t) (uint64_t) result->value;
  const char_t *pText = input + length;
  pText += pass_space(pText);
  if (*pText == ']') {
    result->type = enum_WIDTH;
    result->value = (void *) (uint64_t) value;
    return (pText + 1 - input);
  } else if (*pText++ != '-') {
    return 0;
  }

  pText += pass_space(pText);
  length = t_NUMBER_adic10(pText, result, allocator);
  pText += pass_space(pText);
  if (length > 0) {  // parse bit field
    if (pText[length] != ']') { return 0; }
    BitField *bitField = allocator->calloc(1, sizeof(BitField));
    bitField->lower = (uint32_t) (uint64_t) result->value;
    bitField->upper = max(value, bitField->lower);
    bitField->lower = min(value, bitField->lower);
    result->type = enum_BIT_FIELD;
    result->value = bitField;
    return (pText + length - input + 1);
  }
  // parse width
  if (strcmp_o(pText, "byte") == lenof("byte")) {
    result->type = enum_WIDTH;
    result->value = (void *) (uint64_t) (value << 3);
    pText += lenof("byte") + pass_space(pText);
    return *pText++ == ']' ? (pText - input) : 0;
  }
  if (strcmp_o(pText, "bit") == lenof("bit")) {
    result->type = enum_WIDTH;
    result->value = (void *) (uint64_t) value;
    pText += lenof("bit") + pass_space(pText);
    return *pText++ == ']' ? (pText - input) : 0;
  }
  return 0;
}

uint32_t tokenize_symbol_LPAREN(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  uint32_t length = t_NUMBER_adic10(pText, result, allocator);
  if (length == 0) { return 0; }
  pText += length + pass_space(pText);
  if (*pText++ != '-') { return 0; }
  if (strcmp_o(pText, "tick") != lenof("tick")) { return 0; }
  result->type = enum_TIME_TICK;
  pText += lenof("tick") + pass_space(pText);
  return *pText++ == ')' ? (pText - input + 1) : 0;
}

uint32_t tokenize_symbol_LSQUARE(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  const char_t *pText = input;
  pText += pass_space(pText);
  if ('0' <= pText[0] && pText[0] <= '9') {
    uint32_t length = tokenize_startswith_number(pText, result, allocator);
    if (length > 0) { return length + 1; }
  }
  if (strcmp_o(pText, "...") == lenof("...")) {
    result->type = enum_BIT_FIELD;
    result->value = nullptr;
    pText += lenof("...") + pass_space(pText);
    return *pText++ == ']' ? pText - input + 1 : 0;
  }
  result->type = enum_LEFT_SQUARE_BRACKET;
  result->value = nullptr;
  return 1;
}

uint32_t tokenize_number(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  uint32_t length = 0;
  if ('0' == *input) {
    switch (input[1]) {
      case 'x':
      case 'X': {
        length = t_NUMBER_adic16(input + 2, result, allocator);
        return length == 0 ? 0 : length + 2;
      }
      case 'o':
      case 'O': {
        length = t_NUMBER_adic8(input + 2, result, allocator);
        return length == 0 ? 0 : length + 2;
      }
      case 'b':
      case 'B': {
        length = t_NUMBER_adic2(input + 2, result, allocator);
        return length == 0 ? 0 : length + 2;
      }
      default: {
      }
    }
  }
  return t_NUMBER_adic10(input, result, allocator);
}

const uint32_t TERMINAL_TYPE_LITERALS[] = {
    enum_LEFT_BRACKET, enum_RIGHT_BRACKET,        enum_COLON, enum_SEMICOLON,
    enum_EQUAL,        enum_RIGHT_SQUARE_BRACKET, enum_COMMA, enum_DOT,
};
inline uint32_t single_tokenize(
    const char_t * const input, Terminal * const result, const Allocator * const allocator
) {
  switch (*input) {
    case 'i': {
      return tokenize_letter_i(input + 1, result, allocator);
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
    case '$': {
      result->type = enum_MEM_KEY;
      result->value = (void *) (uint64_t) MEM_BASE;
      return 1;
    }
    case '>': {
      result->type = enum_MEM_KEY;
      result->value = (void *) (uint64_t) MEM_OFFSET;
      return 1;
    }
    case '^': {
      result->type = enum_PART_KEY;
      result->value = (void *) (uint64_t) PART_PREFIX;
      return 1;
    }
    case '&': {
      result->type = enum_PART_KEY;
      result->value = (void *) (uint64_t) PART_SUFFIX;
      return 1;
    }
    case '~': {
      result->type = enum_PART_KEY;
      result->value = (void *) (uint64_t) PART_PRINCIPAL;
      return 1;
    }
    default: {
    }
  }
  uint32_t length = tokenize_number(input, result, allocator);
  if (length > 0) { return length; }
  length = t_IDENTIFIER(input, result, allocator);
  if (length > 0) { return length; }
  length = stridx_o(*input, "{}:;=],.");
  if (length < lenof("{}:;=],.")) {
    result->type = TERMINAL_TYPE_LITERALS[length];
    result->value = nullptr;
    return 1;
  }
  return 0;
}

uint32_t pass_space(const char_t * const input) {
  const char_t *pText = input;
  while (*pText && stridx_o(*pText, " \t\n\f\v\r") < lenof(" \t\n\f\v\r")) { pText++; }
  return pText - input;
}

const Terminal *tokenize(
    const char_t * const input, uint32_t *cost, uint32_t *n_tokens,
    const Allocator * const allocator
) {  // NOLINT(*-easily-swappable-parameters)
  const char_t *pText = input;
  Array *terminals = Array_new(sizeof(Terminal), allocator);
  Terminal terminal = {};
  pText += pass_space(pText);
  while (*pText) {
    *cost = single_tokenize(pText, &terminal, allocator);
    if (0 == *cost) { break; }
    pText += *cost;
    pText += pass_space(pText);
    Array_append(terminals, &terminal, 1);
  }
  if (0 == *pText) {
    terminal.type = enum_TERMINATOR;
    terminal.value = nullptr;
    Array_append(terminals, &terminal, 1);
  }
  *cost = (uint32_t) (pText - input);
  *n_tokens = Array_length(terminals);
  const Terminal *pTerminals = (*n_tokens == 0) ? nullptr : Array_get(terminals, 0);
  Array_destroy(terminals);
  return pTerminals;
}

inline const char_t *get_name(uint16_t type) {
  return MACHINE_TOKEN_NAMES[type];
}
