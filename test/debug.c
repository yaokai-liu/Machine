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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHAR 1024

int main() {
  char_t testString[MAX_CHAR] = {};
  uint32_t cost = 0, n_tokens = 0;
  uint32_t lineno = 0, column = 0;
  FILE *pFile = fopen("/mnt/d/Codelib/machine/liu-machine/demo.mm", "r");
  if (!pFile) { return -1; }
  uint32_t length = fread(testString, sizeof(char_t), MAX_CHAR, pFile);
  printf("read %u characters from file.\n", length);
  if (fclose(pFile)) {
    printf("failed to close file.\n");
    return -2;
  }
  //    const char_t * str = string_t("[23-12]");
  const Terminal *terminals =
      tokenize(testString, &cost, &n_tokens, &lineno, &column, &STDAllocator);
  if (!n_tokens) {
    printf("failed to lex at <%d>.\n", cost);
    STDAllocator.free((void *) terminals);
    return -3;
  }
  for (uint32_t i = 0; i < n_tokens; i++) {
    uint32_t t_line = terminals[i].lineno;
    uint32_t t_start = terminals[i].column;
    uint32_t t_end = (terminals[i].length > 0) ? terminals[i].column + terminals[i].length - 1 : 0;
    printf(
        "(line: %u, col: %u-%u, type: %s, value: %p)\n", t_line, t_start, t_end,
        get_name(terminals[i].type), terminals[i].value
    );
  }
  const Machine *machine = parse(terminals, &cost, get_codegen, &STDAllocator);
  if (!machine) {
    printf("failed to parse.\n");
    for (uint32_t i = cost; i < n_tokens; i++) {
      releaseToken(terminals[i].value, terminals[i].type, &STDAllocator);
    }
    STDAllocator.free((void *) terminals);
    return -4;
  }
  char_t string[512] = {};
  memcpy(string, machine->name->ptr, machine->name->len);
  string[machine->name->len] = '\0';
  printf("machine %s\n", string);

  char_t *outputs = Array_real_addr(GContext_getOutputBuffer(machine->context), 0);
  printf("%s\n", outputs);

  releaseMachine((Machine *) machine, &STDAllocator);
  STDAllocator.free((void *) machine);
  STDAllocator.free((void *) terminals);
  return 0;
}
