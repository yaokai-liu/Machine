
/**
 * Project Name: machine
 * Module Name: codegen/C/template
 * Filename: encoding.gen.c
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include <stdint.h>

#define min(a, b) ((a) < (b)) ? (a) : (b)

#define UINT_N_MAX(n_bits)    ((1LLU << (n_bits)) - 1)
#define LOW_BITS(val, n_bits) ((val) & UINT_N_MAX(n_bits))
#define MASK_BITS(bl, bu)     (UINT_N_MAX(bu) - UINT_N_MAX(bl))

#define numSetBits(num, bl, bu, val) \
  (((num) & ~MASK_BITS(bu, bl)) | (LOW_BITS(val, (bu) - (bl)) << (bl)))

#define setEncodingNumber(val) do { number = (val); } while (false)

#define pushInstrBytes(_count) do {             \
  uint32_t last = min(_count + index, size);    \
  for (uint32_t i = index; i < last; i++) {     \
    bytes[i] = number & 0xFF;                   \
    number >>= 8;                               \
  }                                             \
  index = last;                                 \
} while (false)

#define pushEncodingNumber(val, count) do {   \
  setEncodingNumber(val);                     \
  pushInstrBytes(count);                      \
} while (false)

uint32_t encoding_${InstrForm.Id}(${InstrFormArgs}, Array *buffer) {
    const size = ${InstrFormSize};
    uint8_t bytes[${InstrFormSize}] = {};
    uint64_t number = 0;
    uint32_t index = 0;

    ${InstrFormCodegen}
    pushEncodingNumber(0x56, 1);
    numSetBits(number, 0, 6, (local >> 6) & UINT_N_MAX(6));
    numSetBits(number, 6, 11, -1);
    numSetBits(number, 11, 17, (local >> 0) & UINT_N_MAX(6));
    numSetBits(number, 17, 23, -1);
    numSetBits(number, 23, 32, gax);
    numSetBits(number, 32, 56, -1);
    numSetBits(number, 56, 64, gax);
    pushInstrBytes(8);
    pushEncodingNumber(0x37, 1);

    Array_append(buffer, bytes, sizeof(bytes));
}
