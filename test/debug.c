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
#include "terminal.h"
#include "tokenize.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "target.h"
#include "parse.h"

#define MAX_CHAR 1024


int main() {
  char_t testString[MAX_CHAR] = {};
  uint32_t cost = 0;
  uint32_t n_tokens = 0;
  FILE * pFile = fopen("/mnt/d/Codelib/machine/liu-machine/demo.mm", "r");
  if (!pFile) { return  -1; }
  uint32_t length = fread(testString, sizeof(char_t), MAX_CHAR, pFile);
  printf("read %u characters from file.\n", length);
  if (fclose(pFile)) {
      printf("failed to close file.\n");
      return -2;
  }
  const Terminal *terminals = tokenize(testString, &cost, &n_tokens, &STDAllocator);
  if (!n_tokens) {
      printf("failed to lex at <%d>.\n", cost);
      return -3;
  }
  for (uint32_t i = 0; i < n_tokens; i++) { printf("%s :: %p\n", get_name(terminals[i].type), terminals[i].value); }
  const Machine * machine = parse(terminals, &STDAllocator);
  if (!machine) {
      printf("failed to parse.\n");
      return -4;
  }
  char_t string[512] = {};
  memcpy(string, machine->name->ptr, machine->name->len);
  string[machine->name->len] = '\0';
  printf("machine %s", string);
  return 0;
}
