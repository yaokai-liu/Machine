/**
 * Project Name: machine
 * Module Name: codegen/C
 * Filename: target.h
 * Creator: Yaokai Liu
 * Create Date: 2024-08-26
 * Copyright (c) 2024 Yaokai Liu. All rights reserved.
 **/

#include "array.h"
#include "codegen.h"
#include "context.h"
#include "enum.h"
#include "target.h"
#include "terminal.h"
#include "tokens.gen.h"
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define push_string(s) do { \
    Array_append(buffer, s, strlen(s)); \
} while(false)

#define numberToInstrBytes(number, width) do { \
  push_string("pushInstrBytes("); \
  while (width) { \
    char_t buf[0x20] = {}; \
    uint8_t byte = (uint8_t)(number & 0xFF); \
    sprintf(buf, "0x%02X", byte); \
    push_string(buf); \
    width -= 8; \
    number >>= 8; \
    if (width) { \
      push_string(", "); \
    } \
  }; \
  push_string(");"); \
} while (false)


#define namedToInstrBytes(name, width) do { \
  char_t buf[0x20] = {}; \
  push_string("pushInstrNamed("); \
  push_string(name); \
  sprintf(buf, "%d", width / 8); \
  push_string(buf); \
  push_string(");"); \
} while (false)


#define bitFieldToInstrBytes(name, bf, width) do { \
  char_t buf[0x20] = {}; \
  push_string("pushInstrBitField("); \
  push_string(name); \
  sprintf(buf, "%d", bf->lower); \
  push_string(buf); \
  sprintf(buf, "%d", bf->upper); \
  push_string(buf); \
  sprintf(buf, "%d", width / 8); \
  push_string(buf); \
  push_string(");"); \
} while (false)



int32_t codegen_instr_form(GContext * context, InstrForm * form, Array* buffer) {
  const uint32_t pre_len = Array_length(buffer);
  uint32_t width = form->parts[PART_PREFIX - 1].width;
  const Layout * layout = form->parts[PART_PREFIX - 1].layout;
  switch (layout->type) {
    case enum_Evaluable: {
      Evaluable * evalable = layout->target;
      switch (evalable->type) {
        case enum_NUMBER: {
          uint64_t number = (uint64_t) layout->target;
          numberToInstrBytes(number, width);
          break;
        }
        case enum_IDENTIFIER: {
          Entry * entry = GContext_findRefer(context, evalable->lhs);
          switch (entry->type) {
            case enum_Register: {
              uint64_t number = ((Register *) entry->target)->code;
              numberToInstrBytes(number, width);
              break;
            }
            case enum_Memory:
            case enum_Immediate: {
              const char_t * name = ((Identifier *)evalable->lhs)->ptr;
              namedToInstrBytes( name, width);
              break;
            }
            case enum_Set: {
              // TODO: codegen for set
            }
          }
          break;
        }
        case enum_BIT_FIELD: {
          Entry * entry = GContext_findRefer(context, evalable->lhs);
          BitField * bf = GContext_findRefer(context, evalable->rhs);
          switch (entry->type) {
            case enum_Register: {
              uint64_t number = ((Register *) entry->target)->code;
              number = (number >> bf->lower) & ((1 << (bf->upper - bf->lower + 1)) - 1);
              numberToInstrBytes(number, width);
              break;
            }
            case enum_Memory:
            case enum_Immediate: {
              const char_t * name = ((Identifier *)evalable->lhs)->ptr;
              bitFieldToInstrBytes(name, bf, width);
              break;
            }
            case enum_Set: {
              // TODO: codegen for set
            }
          }
          break;
        }
        case enum_MEM_KEY: {
          
        }
      }
      break;
    }
    case enum_MappingItems: {

    }
  }
  return Array_length(buffer) - pre_len;
}

int32_t codegen_instruction(GContext *, Instruction*, Array *) {
  // TODO: codegen instruction
  return 0;
}