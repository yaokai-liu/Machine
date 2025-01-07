/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "encoding.h"
#include <stdio.h>
#include <string.h>

const char_t ENCODING_DEC_FMT[] = "uint32_t encoding_%s(%s, Array *buffer)";

const char_t ENCODING_DEF_FMT_HEAD[] = "{\n"
                                       "  const uint32_t size = %d;\n"
                                       "  uint8_t bytes[%d] = {};\n"
                                       "  uint64_t number = 0;\n"
                                       "  uint32_t index = 0;\n"
                                       "\n";

const char_t ENCODING_DEF_FMT_TAIL[] = "  Array_append(buffer, bytes, sizeof(bytes));\n"
                                       "  return size;\n"
                                       "}\n";

#define push_string(s) \
  do { Array_append(generator->buffer, s, strlen(s)); } while (false)

#define gen_encoding_dec_core(name, form)                         \
  do {                                                            \
    push_string("uint32_t encoding_");                            \
    push_string((name)->ptr);                                     \
    const uint32_t n_args = Array_length((form)->pattern->args);  \
    const Identifier *args = Array_real_addr((form)->pattern->args, 0); \
    for (uint32_t j = 0; j < n_args; j++) {                       \
      const Identifier *arg = &args[i];                           \
      push_string("_");                                           \
      push_string(arg->ptr);                                      \
    }                                                             \
    push_string("(");                                             \
    for (uint32_t j = 0; j < n_args; j++) {                       \
      const Identifier *arg = &args[i];                           \
      push_string("uint64_t ");                                   \
      push_string(arg->ptr);                                      \
      push_string(", ");                                          \
    }                                                             \
    push_string("Array *buffer)");                                \
  } while (false)

int32_t gen_encoding_dec(
    Generator *generator, Identifier *names[], InstrForm *forms[], uint32_t n_forms
) {
  for (uint32_t i = 0; i < n_forms; ++i) {
    gen_encoding_dec_core(names[i], forms[i]);
    push_string(";\n");
  }
  return 0;
}

int32_t gen_encoding_def(
    Generator *generator, Identifier *names[], InstrForm *forms[], uint32_t n_forms
) {
  char_t head_buffer[sizeof(ENCODING_DEF_FMT_HEAD) + 64];
  Array *buffer = Array_new(sizeof(char_t), -1, generator->allocator);
  for (uint32_t i = 0; i < n_forms; ++i) {
    const uint32_t n_bytes = forms[i]->width / 8;
    gen_encoding_dec_core(names[i], forms[i]);
    sprintf(head_buffer, ENCODING_DEF_FMT_HEAD, n_bytes, n_bytes);
    push_string(head_buffer);
    codegen_instr_form(generator->context, forms[i], buffer);
    uint32_t size = Array_length(buffer);
    char_t *ptr = Array_real_addr(buffer, 0);
    Array_append(generator->buffer, ptr, size);
    push_string(ENCODING_DEF_FMT_TAIL);
    Array_reset(buffer, nullptr);
  }
  Array_destroy(buffer);
  return 0;
}
