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
#include "terminal.h"
#include "tokenize.h"
#include "tokens.gen.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define print(ndx)                                                                   \
  do {                                                                               \
    char_t c = '\0';                                                                 \
    Array_append(Generator_getOutputBuffer(generator, ndx), &c, 1);                  \
    char_t *outputs = Array_real_addr(Generator_getOutputBuffer(generator, ndx), 0); \
    fprintf(pFile, "%s\n", outputs);                                                 \
  } while (0)

int main() {
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const char_t *filepath = "/mnt/d/Codelib/machine/preset/x64";
  FILE *pFile = fopen(filepath, "r");
  if (!pFile) { return -1; }
  fseek(pFile, 0, SEEK_END);
  uint32_t length = ftell(pFile);
  fseek(pFile, 0, SEEK_SET);
  char_t *testString = STDAllocator.malloc(sizeof(char_t) * (length + 1));
  fread(testString, sizeof(char_t), length, pFile);
  printf("read %u characters from file.\n\n", length);
  if (fclose(pFile)) {
    printf("failed to close file.\n");
    return -2;
  }
  clock_t start = clock();

  //    const char_t * str = string_t("[23-12]");
  const Terminal *terminals =
      tokenize(testString, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  if (terminals[n_tokens - 1].type != enum_TERMINATOR) {
    printf("failed to lex %s:%u:%u\n", filepath, lineno, column);
    printf("unknown character '%c'\n", testString[cost]);
    STDAllocator.free((void *) terminals);
    return -3;
  }
  //  for (uint32_t i = 0; i < n_tokens; i++) {
  //    uint32_t t_line = terminals[i].lineno;
  //    uint32_t t_start = terminals[i].column;
  //    uint32_t t_end = (terminals[i].length > 0) ? terminals[i].column + terminals[i].length - 1 :
  //    0; printf(
  //        "(line: %u, col: %u-%u, type: %s, value: %p)\n", t_line, t_start, t_end,
  //        get_name(terminals[i].type), terminals[i].value
  //    );
  //  }
  const char_t *err_msg = nullptr;
  const Machine *machine = parse(terminals, &cost, &err_msg, &STDAllocator);
  if (!machine) {
    printf("failed to parse. %s\n", err_msg);
    for (uint32_t i = cost; i < n_tokens; i++) {
      releaseToken(terminals[i].value, terminals[i].type, &STDAllocator);
    }
    STDAllocator.free((void *) terminals);
    return -4;
  }
  Generator *generator = Generator_new(&STDAllocator);
  codegen(generator, machine);
  clock_t end = clock();

  //  char_t string[512] = {};
  //  memcpy(string, machine->name->ptr, machine->name->len);
  //  string[machine->name->len] = '\0';
  //  printf("machine %s\n", string);

  pFile = fopen("machine-x64.h", "w");
  if (!pFile) { return -1; }
  print(GenBuf_exports);
  fclose(pFile);

  pFile = fopen("machine-x64.c", "w");
  if (!pFile) { return -1; }
  print(GenBuf_includes);
  print(GenBuf_macros);
  print(GenBuf_enums);
  print(GenBuf_types);
  print(GenBuf_declares);
  print(GenBuf_definitions);
  print(GenBuf_tables);
  fclose(pFile);

  printf("time cost: %fms\n", (double) (end - start) / CLOCKS_PER_SEC * 1000);

  releaseMachine((Machine *) machine, &STDAllocator);
  STDAllocator.free((void *) machine);
  STDAllocator.free((void *) terminals);
  Generator_destroy(generator);
  return 0;
}
