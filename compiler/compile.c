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
 * Module Name: test
 * Filename: debug.c
 * Creator: Yaokai Liu
 * Create Date: 2024-09-14
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "allocator.h"
#include "char_t.h"
#include "enum.h"
#include "generate.h"
#include "parse/parse.h"
#include "parse/target.h"
#include "tokenize/target.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
  #define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
#endif
#include <string.h>
#include <time.h>

#define print(ndx)                                                                     \
  do {                                                                                 \
    char_t c = '\0';                                                                   \
    Array_append(CGenerator_getOutputBuffer((CGenerator *) generator, ndx), &c, 1);    \
    char_t *outputs =                                                                  \
        Array_real_addr(CGenerator_getOutputBuffer((CGenerator *) generator, ndx), 0); \
    fprintf(file, "%s\n", outputs);                                                    \
  } while (0)

int expand_macro(char_t *srcpath, char_t *output_path);

int main(int argc, char *argv[]) {
  char_t *srcname;
  char_t *outname;
  char_t srcpath[1024] = {};
  char_t headpath[1024] = {};
  char_t libpath[1024] = {};
  char_t *year = nullptr;
  char_t *cr_holder = nullptr;

  if (argc < 2) {
    fprintf(stderr, "source file required.\n");
    return -1;
  }
  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    fprintf(stdout, "%s <file> [<output>] [options]\n", argv[0]);
    fprintf(stdout, "\t convert machine <file> to '<output>.h' and '<output>.c'\n");
    fprintf(stdout, "\t if '<output>' not set, output would be the machine's name\n");
    fprintf(stdout, "\t [options]\n");
    fprintf(stdout, "\t \t -y | --year <year>\tset copyright year\n");
    fprintf(stdout, "\t \t -a | --author <author>\tset copyright holder\n");
    fprintf(stdout, "%s [-h | --help] \tshow this help\n", argv[0]);
    fprintf(stdout, "%s -E \t only expand macros instead compile\n", argv[0]);
    return 0;
  }
  srcname = argv[1];
  realpath(srcname, srcpath);

  int arg_ndx = 2;
  if (argc > 2 && argv[2][0] != '-') {
    outname = argv[2];
    realpath(outname, headpath);
    realpath(outname, libpath);
    arg_ndx++;
  } else {
    outname = strrchr(srcname, '/');
    outname = outname ? outname + 1 : srcname;
    realpath(outname, headpath);
    realpath(outname, libpath);
  }
  uint32_t len = strlen(headpath);
  strcpy(headpath + len, ".h");
  strcpy(libpath + len, ".c");
  for (; arg_ndx < argc; arg_ndx++) {
    if (strcmp(argv[arg_ndx], "-y") == 0 || strcmp(argv[arg_ndx], "--year") == 0) {
      if (arg_ndx + 1 >= argc) {
        fprintf(stderr, "copyright year required.\n");
        return -1;
      } else {
        year = argv[arg_ndx + 1];
        arg_ndx++;
      }
    } else if (strcmp(argv[arg_ndx], "-a") == 0 || strcmp(argv[arg_ndx], "--author") == 0) {
      if (arg_ndx + 1 >= argc) {
        fprintf(stderr, "copyright holder required.\n");
        return -1;
      } else {
        cr_holder = argv[arg_ndx + 1];
        arg_ndx++;
      }
    } else if (strcmp(argv[arg_ndx], "-E") == 0) {
      return expand_macro(srcpath, headpath);
    } else {
      fprintf(stderr, "wrong count of arguments.\n");
      return -1;
    }
  }

  FILE *file = fopen(srcpath, "r");
  if (!file) {
    fprintf(stderr, "unable to read source file, please check if it exists.\n");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char_t *text = STDAllocator.calloc(size + 1, sizeof(char_t));
  fread(text, sizeof(char_t), size, file);
  if (fclose(file)) {
    fprintf(stderr, "failed to close file.\n");
    return -2;
  }
  Array *ident_array = Array_new(sizeof(Identifier), enum_IDENTIFIER, &STDAllocator);
  Tokenizer *tokenizer = Tokenizer_new(text, ident_array, &STDAllocator);
  CGenerator *generator = Generator_new_C(ident_array, &STDAllocator);

  clock_t start = clock();

  ErrInfo err_info = {};
  const Machine *machine = parse(tokenizer, &err_info, &STDAllocator);
  if (!machine) {
    fprintf(stderr, "failed to parse:\n");
    Array *frame_array = Array_new(sizeof(TokenPos), -1, &STDAllocator);
    Tokenizer_frame_pos_to_array(tokenizer, frame_array);
    const TokenPos *positions = Array_first_real(frame_array);
    const uint32_t last = Array_length(frame_array) - 1;
    for (uint32_t i = last - 1; i < last; i--) {
      fprintf(stderr, "  at %s:%d:%d\n", srcpath, positions[i][1].lineno, positions[i][1].column);
    }
    fprintf(stderr, "  at %s:%d:%d\n", srcpath, err_info.pos[1].lineno, err_info.pos[1].column);
    fprintf(stderr, "  %s\n", err_info.msg);
    STDAllocator.free(text);
    return 0;
  }
  Generator_setCopyright((Generator *) generator, outname, headpath, libpath, cr_holder, year);
  codegen((Generator *) generator, machine);

  clock_t end = clock();

  file = fopen(headpath, "w");
  if (!file) { return -1; }
  print(GenC_exports);
  fclose(file);

  file = fopen(libpath, "w");
  if (!file) { return -1; }
  print(GenC_includes);
  print(GenC_macros);
  print(GenC_enums);
  print(GenC_types);
  print(GenC_declares);
  print(GenC_definitions);
  print(GenC_tables);
  fclose(file);

  fprintf(stdout, "time cost: %fms\n", (double) (end - start) / CLOCKS_PER_SEC * 1000);

  releaseMachine((Machine *) machine, &STDAllocator);
  STDAllocator.free((void *) machine);
  Generator_destroy((Generator *) generator);
  releasePrimeArray(ident_array);
  Tokenizer_destroy(tokenizer);
  STDAllocator.free(text);

  return 0;
}

extern const char_t * MACHINE_TERMINAL_STRINGS[];
const char_t *ARITH_SYMBOLS[] = {
    [AB_ADD - 128] = "+",
    [AB_SUB - 128] = "-",
    [AB_MUL - 128] = "*",
    [AB_DIV - 128] = "/",
    [AB_MOD - 128] = "%",
    [AB_OR - 128] = "|",
    [AB_AND - 128] = "&",
    [AB_XOR - 128] = "^",
    [AB_LSH - 128] = "<<",
    [AB_RSH - 128] = ">>",
    [AS_INV - 128] = "~",
    [CB_LT - 128] = "<",
    [CB_LE - 128] = "<=",
    [CB_GT - 128] = ">",
    [CB_GE - 128] = ">=",
    [CB_EQ - 128] = "==",
    [CB_NE - 128] = "!=",
    [AS_ID - 128] = "",
    [CB_IN - 128] = "in",
};
int expand_macro(char_t *srcpath, char_t *output_path) {
  FILE *file = fopen(srcpath, "r");
  if (!file) {
    fprintf(stderr, "unable to read source file, please check if it exists.\n");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char_t *text = STDAllocator.calloc(size + 1, sizeof(char_t));
  fread(text, sizeof(char_t), size, file);
  if (fclose(file)) {
    fprintf(stderr, "failed to close file.\n");
    return -2;
  }
  file = fopen(output_path, "w");
  if (!file) { return -1; }

  Array *ident_array = Array_new(sizeof(Identifier), enum_IDENTIFIER, &STDAllocator);
  Tokenizer *tokenizer = Tokenizer_new(text, ident_array, &STDAllocator);
  Token token = {}; ErrInfo errInfo = {};
  do {
    Tokenizer_next(tokenizer, &token, &errInfo);
    if (token.type == enum_NUMBER) {
      fprintf(file, " %llu ", (uint64_t) token.value);
    }
    else if (token.type == enum_WIDTH) {
      fprintf(file, " [%llu] ", (uint64_t) token.value);
    }
    else if (token.type == enum_BIT_FIELD) {
      uint32_t upper = (uint64_t) token.value;
      uint32_t lower = (uint64_t) token.value >> 32;
      fprintf(file, " [%llu-%llu] ", upper, lower);
    }
    else if (token.type == enum_TIME_TICK) {
      fprintf(file, " (%llu) ", (uint64_t) token.value);
    }
    else if (token.type == enum_IDENTIFIER) {
      char_t * ident = Array_virt2real(ident_array, token.value);
      fprintf(file, " %s ", ident);
    }
    else if (token.type < MAX_REAL_TOKEN && MACHINE_TERMINAL_STRINGS[token.type]) {
      fprintf(file, " %s ", MACHINE_TERMINAL_STRINGS[token.type]);
    }
    else {
      switch (token.type) {
        case enum_COND_BIN_OP:
        case enum_COND_SIN_OP:
        case enum_ARITH_0_BIN_OP:
        case enum_ARITH_0_SIN_OP:
        case enum_ARITH_1_BIN_OP:
        case enum_ARITH_1_SIN_OP:
        case enum_ARITH_2_BIN_OP:
        case enum_ARITH_2_SIN_OP:
        case enum_ARITH_3_SIN_OP:{
          uint32_t a_type = (uint64_t) token.value;
          fprintf(file, " %s ", ARITH_SYMBOLS[a_type - 128]);
          break;
        }
        default:{
          fprintf(file, " □ ");
        }
      }
    }
  } while (token.type != enum_TERMINATOR);
  fclose(file);
  return 0;
}
