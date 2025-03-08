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
#include "generate.h"
#include "parse.h"
#include "target.h"
#include "tokens.gen.h"
#include "terminal.h"
#include "tokenize.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
  #define realpath(N, R) _fullpath((R), (N), _MAX_PATH)
#endif
#include <string.h>
#include <time.h>

#define print(ndx)                                                                   \
  do {                                                                               \
    char_t c = '\0';                                                                 \
    Array_append(Generator_getOutputBuffer(generator, ndx), &c, 1);                  \
    char_t *outputs = Array_real_addr(Generator_getOutputBuffer(generator, ndx), 0); \
    fprintf(file, "%s\n", outputs);                                                  \
  } while (0)

int main(int argc, char *argv[]) {
  char_t *srcname = nullptr;
  char_t *outname = nullptr;
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
    return 0;
  }
  srcname = argv[1];
  realpath(srcname, srcpath);

  int i = 2;
  if (argc > 2 && argv[2][0] != '-') {
    outname = argv[2];
    realpath(outname, headpath);
    realpath(outname, libpath);
    i++;
  } else {
    outname = strrchr(srcname, '/');
    outname = outname ? outname + 1 : srcname;
    realpath(outname, headpath);
    realpath(outname, libpath);
  }
  uint32_t len = strlen(headpath);
  strcpy(headpath + len, ".h");
  strcpy(libpath + len, ".c");
  for (; i < argc; i++) {
    if (strcmp(argv[i], "-y") == 0 || strcmp(argv[i], "--year") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "copyright year required.\n");
        return -1;
      } else {
        year = argv[i + 1];
        i++;
      }
    } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--author") == 0) {
      if (i + 1 >= argc) {
        fprintf(stderr, "copyright holder required.\n");
        return -1;
      } else {
        cr_holder = argv[i + 1];
        i++;
      }
    } else {
      fprintf(stderr, "wrong count of arguments.\n");
      return -1;
    }
  }

  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  FILE *file = fopen(srcpath, "r");
  if (!file) {
    fprintf(stderr, "unable to read source file, please check if it exists.\n");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);
  char_t *text = STDAllocator.calloc(size + 1, sizeof(char_t));
  len = fread(text, sizeof(char_t), size, file);
  if (fclose(file)) {
    fprintf(stderr, "failed to close file.\n");
    return -2;
  }
  clock_t start = clock();
  const Terminal *terminals = tokenize(text, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  STDAllocator.free(text);
  if (!terminals || terminals[n_tokens - 1].type != enum_TERMINATOR) {
    fprintf(stderr, "failed to lex %s:%u:%u\n", srcpath, lineno, column);
    fprintf(stderr, "unknown character '%c'\n", text[cost]);
    STDAllocator.free((void *) terminals);
    return -3;
  }
  const char_t *err_msg = nullptr;
  const Machine *machine = parse(terminals, &cost, &err_msg, &STDAllocator);
  if (!machine) {
    fprintf(
        stderr, "failed to parse %s:%d:%d:%d. %s\n", srcpath, terminals[cost - 1].lineno + 1,
        terminals[cost - 1].column + 1, terminals[cost - 1].length, err_msg
    );
    for (uint32_t j = cost; j < n_tokens; j++) {
      releaseToken(terminals[j].value, terminals[j].type, &STDAllocator);
    }
    STDAllocator.free((void *) terminals);
    return -4;
  }
  Generator *generator = Generator_new(&STDAllocator);
  Generator_setCopyright(generator, outname, headpath, libpath, cr_holder, year);
  codegen(generator, machine);
  clock_t end = clock();

  file = fopen(headpath, "w");
  if (!file) { return -1; }
  print(GenBuf_exports);
  fclose(file);

  file = fopen(libpath, "w");
  if (!file) { return -1; }
  print(GenBuf_includes);
  print(GenBuf_macros);
  print(GenBuf_enums);
  print(GenBuf_types);
  print(GenBuf_declares);
  print(GenBuf_definitions);
  print(GenBuf_tables);
  fclose(file);

  fprintf(stdout, "time cost: %fms\n", (double) (end - start) / CLOCKS_PER_SEC * 1000);

  releaseMachine((Machine *) machine, &STDAllocator);
  STDAllocator.free((void *) machine);
  STDAllocator.free((void *) terminals);
  Generator_destroy(generator);
  return 0;
}
