/**
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

#define MAX_CHAR 4096

#define print(ndx)                                                                    \
  do {                                                                               \
    char_t c = '\0';                                                                 \
    Array_append(Generator_getOutputBuffer(generator, ndx), &c, 1);                  \
    char_t *outputs = Array_real_addr(Generator_getOutputBuffer(generator, ndx), 0); \
    fprintf(pFile, "%s\n", outputs);                                                 \
  } while (0)

int main() {
  char_t testString[MAX_CHAR] = {};
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  const char_t *filepath = "/mnt/d/Codelib/machine/liu-machine/demo.mm";
  FILE *pFile = fopen(filepath, "r");
  if (!pFile) { return -1; }
  uint32_t length = fread(testString, sizeof(char_t), MAX_CHAR, pFile);
  printf("read %u characters from file.\n\n", length);
  if (fclose(pFile)) {
    printf("failed to close file.\n");
    return -2;
  }
  int start = clock();

  //    const char_t * str = string_t("[23-12]");
  const Terminal *terminals =
      tokenize(testString, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  if (terminals[n_tokens - 1].type != enum_TERMINATOR) {
    printf("falied to lex %s:%u:%u\n", filepath, lineno, column);
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
  int end = clock();

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
