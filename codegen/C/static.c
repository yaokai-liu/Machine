/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: static.c
 * Creator: Yaokai Liu
 * Create Date: 2024-12-11
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "static.h"
#include <stdio.h>
#include <string.h>

const char_t HEADER_FMT[] =
    "/**\n"
    " * Filename: %s\n"
    " *\n"
    " * This file is generated by xMachine, adopts the MIT License:\n"
    " *      MIT License\n"
    " *      Copyright (c) %d %s\n"
    " *\n"
    " *      Permission is hereby granted, free of charge, to any person obtaining a copy\n"
    " *      of this software and associated documentation files (the \"Software\"), to deal\n"
    " *      in the Software without restriction, including without limitation the rights\n"
    " *      to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n"
    " *      copies of the Software, and to permit persons to whom the Software is\n"
    " *      furnished to do so, subject to the following conditions:\n"
    " *\n"
    " *      The above copyright notice and this permission notice shall be included in all\n"
    " *      copies or substantial portions of the Software.\n"
    " *\n"
    " *      THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
    " *      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
    " *      FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n"
    " *      AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
    " *      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
    " *      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
    " *      SOFTWARE.\n"
    " *\n"
    " * *The following appendix is not included in the License.*\n"
    " *\n"
    " * Appendix\n"
    " * ==================== xMachine =======================\n"
    " * xMachine - A Backend Generator for Compilers\n"
    " * xMachine adopts the GNU AFFERO GENERAL PUBLIC LICENSE.\n"
    " * Copyright (c) 2024 Yaokai Liu. All rights reserved.\n"
    " * =====================================================\n"
    " *\n"
    " **/";

const char_t MACROS[] = "#define min(a, b)             ((a) < (b)) ? (a) : (b)\n"
                        "#define UINT_N_MAX(n_bits)    ((1LLU << (n_bits)) - 1)\n"
                        "#define LOW_BITS(val, n_bits) ((val) & UINT_N_MAX(n_bits))\n"
                        "#define MASK_BITS(bl, bu)     (UINT_N_MAX(bu) - UINT_N_MAX(bl))\n"
                        "#define numSetBits(num, bl, bu, val) \\\n"
                        "  (((num) & ~MASK_BITS(bl, bu)) | (LOW_BITS(val, (bu) - (bl)) << (bl)))\n"
                        "#define setEncodingNumber(val) \\\n"
                        "  do { number = (val); } while (false)\n"
                        "#define pushInstrBytes(_count)                 \\\n"
                        "  do {                                         \\\n"
                        "    uint32_t last = min(_count + index, size); \\\n"
                        "    for (uint32_t i = index; i < last; i++) {  \\\n"
                        "      bytes[i] = number & 0xFF;                \\\n"
                        "      number >>= 8;                            \\\n"
                        "    }                                          \\\n"
                        "    index = last;                              \\\n"
                        "  } while (false)\n"
                        "#define pushEncodingNumber(val, count) \\\n"
                        "  do {                                 \\\n"
                        "    setEncodingNumber(val);            \\\n"
                        "    pushInstrBytes(count);             \\\n"
                        "  } while (false)\n";

const char_t TYPE_DEFS[] = "typedef struct {\n"
                           "  uint32_t type;\n"
                           "  uint64_t value;\n"
                           "} Entry;\n";

const char_t JUMP_ITEM[] = "struct jump_item {\n"
                           "  uint32_t expected_type;\n"
                           "  uint32_t next_state_index;\n"
                           "};\n";

const char_t JUMP_STATE[] = "struct jump_state {\n"
                            "  uint32_t offset_in_items;\n"
                            "  uint32_t count_of_items;\n"
                            "};";

int32_t set_header(Generator *generator, char_t *filename, int32_t year, char_t *cr_holder) {
  uint32_t fn_len = strlen(filename);
  uint32_t cr_len = strlen(cr_holder);
  char_t *buffer = generator->allocator->malloc(sizeof(HEADER_FMT) + fn_len + cr_len + 32);
  int32_t size = sprintf(buffer, HEADER_FMT, filename, year, cr_holder);
  if (size < 0) { return size; }
  Array_append(generator->buffer, buffer, size);
  generator->allocator->free(buffer);
  return size;
}
