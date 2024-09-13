/**
 * Project Name: machine
 * Module Name: grammar
 * Filename: tokenize.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/
#include "tokenize.h"
#include "enum.h"
#include "terminal.h"
#include "tokens.gen.h"
#include "array.h"

#define lenof(str_literal) ((sizeof str_literal) - 1)

inline uint32_t strcmp_o(const char_t * const str1, const char_t * const str2) {
    int len = 0;
    while (str1[len] == str2[len]) { len ++; }
    return len;
}

inline uint32_t stridx_o(const char_t chr, const char_t * const str) {
    int len = 0;
    while (chr != str[len] && str[len] != 0) { len++; }
    return len;
}

inline uint32_t t_NUMBER_r16(
  const char_t * const input, Terminal * const result, const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' <= *pText && *pText <= '9') {
      value = (value << 4) + (*pText++ - '0');
    } else if ('a' <= *pText && *pText <= 'f') {
      value = (value << 4) + (*pText++ - 'a');
    } else if ('A' <= *pText && *pText <= 'F') {
      value = (value << 4) + (*pText++ - 'A');
    } else {
      break;
    }
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

inline uint32_t t_NUMBER_r10(
  const char_t * const input, Terminal * const result, const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' <= *pText && *pText <= '9') {
      value = (value * 10) + (*pText++ - '0');  // NOLINT(*-magic-numbers)
    } else {
      break;
    }
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

inline uint32_t
  t_NUMBER_r8(const char_t * const input, Terminal * const result, const Allocator * const allocator [[maybe_unused]]) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' <= *pText && *pText <= '7') {
      value = (value * 10) + (*pText++ - '0');  // NOLINT(*-magic-numbers)
    } else {
      break;
    }
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

inline uint32_t
  t_NUMBER_r2(const char_t * const input, Terminal * const result, const Allocator * const allocator [[maybe_unused]]) {
  const char_t *pText = input;
  uint64_t value = 0;
  while (true) {
    if ('0' == *pText || *pText == '1') {
      value = (value << 1) + (*pText++ - '0');
    } else {
      break;
    }
  }
  result->type = enum_NUMBER;
  result->value = (void *) value;
  return pText - input;
}

typedef struct {
  const char_t *ptr;
  uint32_t len;
} String;

inline uint32_t t_IDENTIFIER(
  const char_t * const input, Terminal * const result, const Allocator * const allocator [[maybe_unused]]
) {
  const char_t *pText = input;
  if (('a' <= *pText && *pText <= 'z') || ('A' <= *pText && *pText <= 'Z')) {
    pText++;
  } else {
    return 0;
  }
  while (true) {
    if (('a' <= *pText && *pText <= 'z') || ('A' <= *pText && *pText <= 'Z') || ('0' <= *pText && *pText <= '9')) {
      pText++;
    } else {
      break;
    }
  }
  String *identifier = allocator->calloc(1, sizeof(String));
  identifier->ptr = input;
  identifier->len = pText - input;
  result->type = enum_IDENTIFIER;
  result->value = identifier;
  return identifier->len;
}

#define fn_try_keyword(_kw, _type)                                                                                     \
  uint32_t try_keyword_##_kw(const char_t * const input, Terminal * const result, const Allocator * const allocator) { \
    const char_t pattern[] = string_t(#_kw);                                                                           \
    for (uint32_t i = 2; i < sizeof(pattern) - 1; i++) {                                                               \
      if (input[i - 2] != pattern[i]) {                                                                                \
        String *identifier = allocator->calloc(1, sizeof(String));                                                     \
        identifier->ptr = input - 2;                                                                                   \
        identifier->len = i;                                                                                           \
        result->type = enum_IDENTIFIER;                                                                                \
        result->value = identifier;                                                                                    \
        return i;                                                                                                      \
      }                                                                                                                \
    }                                                                                                                  \
    result->type = enum_##_type;                                                                                       \
    result->value = nullptr;                                                                                           \
    return sizeof(pattern) - 1;                                                                                        \
  }

fn_try_keyword(immediate, IMMEDIATE)
fn_try_keyword(instruction, INSTRUCTION)
fn_try_keyword(machine, MACHINE)
fn_try_keyword(memory, MEMORY)
fn_try_keyword(set, SET)
fn_try_keyword(register, REGISTER)
fn_try_keyword(unsigned, TYPE)

#define fn_fall_through()                                         \
  do {                                                            \
    uint32_t length = t_IDENTIFIER(input - 1, result, allocator); \
    if (length == 0) {                                            \
      String *identifier = allocator->calloc(1, sizeof(String));  \
      identifier->ptr = input - 1;                                \
      identifier->len = 1;                                        \
      result->type = enum_IDENTIFIER;                             \
      result->value = identifier;                                 \
      return 1;                                                   \
    }                                                             \
    return length + 1;                                            \
  } while (0)

uint32_t tokenize_letter_i(const char_t * const input, Terminal * const result, const Allocator * const allocator) {
  switch (*input) {
    case 'm': {
      return try_keyword_immediate(input, result, allocator);
    }
    case 'n': {
      return try_keyword_instruction(input, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_m(const char_t * const input, Terminal * const result, const Allocator * const allocator) {
  switch (*input) {
    case 'a': {
      return try_keyword_machine(input, result, allocator);
    }
    case 'e': {
      return try_keyword_memory(input, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_s(const char_t * const input, Terminal * const result, const Allocator * const allocator) {
  switch (*input) {
    case 'e': {
      return try_keyword_set(input, result, allocator);
    }
    default: fn_fall_through();
  }
}

uint32_t tokenize_letter_u(const char_t *const input, Terminal *const result, const Allocator *const allocator) {
    switch (*input) {
        case 'n': {
            return try_keyword_unsigned(input, result, allocator);
        }
        default: fn_fall_through();
    }
}

typedef struct BitField {
    uint32_t upper;
    uint32_t lower;
} BitField;
uint32_t tokenize_startswith_number(const char_t *const input, Terminal *const result, const Allocator *const allocator) {
    uint32_t length = t_NUMBER_r10(input, result, allocator);
    uint32_t value = (uint32_t) (uint64_t) result->value;
    const char_t * pText = input + length;
    if (*pText != '-') { return 0; }
    pText ++;
    length = t_NUMBER_r10(input, result, allocator);
    if (length > 0) {
        if (pText[length] != ']') { return 0; }
        BitField * bitField = allocator->calloc(1, sizeof(BitField));
        bitField->upper = value;
        bitField->lower = (uint32_t) (uint64_t) result->value;
        result->type = enum_BIT_FIELD;
        result->value = bitField;
        return (pText + length - input);
    } if (strcmp_o(pText, "byte]") == lenof("byte]")) {
        result->type = enum_WIDTH;
        result->value = (void *) (uint64_t) (value << 3);
        return (pText + lenof("byte]") - input);
    } if (strcmp_o(pText, "bit]") == lenof("bit]")) {
        result->type = enum_WIDTH;
        result->value = (void *) (uint64_t) value;
        return (pText + lenof("bit]") - input);
    }
    return 0;
}

uint32_t tokenize_symbol_LPAREN(const char_t *const input, Terminal *const result, const Allocator *const allocator) {
    uint32_t length = t_NUMBER_r10(input, result, allocator);
    if (length == 0) { return 0; }
    const char_t *pText = input + length;
    if (strcmp_o(pText, "tick)") != lenof("tick)")) { return 0; }
    result->type = enum_TIME_TICK;
    return (pText + lenof("tick)") - input);
}


uint32_t tokenize_symbol_LSQUARE(const char_t * const input, Terminal * const result, const Allocator * const allocator) {
    if ('0' <= input[0] && input[0] <= '9') {
        uint32_t length = tokenize_startswith_number(input, result, allocator);
        if (length == 0) { return 0; }
    }
    result->type = enum_LEFT_SQUARE_BRACKET;
    result->value = nullptr;
    return 1;
}

uint32_t tokenize_number(const char_t *const input, Terminal *const result, const Allocator *const allocator) {
    uint32_t length = 0;
    if ('0' == *input) {
        switch (input[1]) {
            case 'x':
            case 'X': {
                length = t_NUMBER_r16(input + 2, result, allocator);
                return length == 0 ? 0 : length + 2;
            }
            case 'o':
            case 'O': {
                length = t_NUMBER_r8(input + 2, result, allocator);
                return length == 0 ? 0 : length + 2;
            }
            case 'b':
            case 'B': {
                length = t_NUMBER_r2(input + 2, result, allocator);
                return length == 0 ? 0 : length + 2;
            }
            default: {}
        }
    }
    return t_NUMBER_r2(input, result, allocator);
}

const uint32_t TERMINAL_TYPE_LITERALS[] = {
        enum_LEFT_BRACKET,
        enum_RIGHT_BRACKET,
        enum_COLON,
        enum_SEMICOLON,
        enum_EQUAL,
        enum_RIGHT_SQUARE_BRACKET,
        enum_COMMA,
        enum_DOT,
};
uint32_t single_tokenize(const char_t * const input, Terminal * const result, const Allocator * const allocator) {
  switch (*input) {
    case 'i': {
      return tokenize_letter_i(input + 1, result, allocator);
    }
    case 'm': {
      return tokenize_letter_m(input + 1, result, allocator);
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
    case '@': {
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
