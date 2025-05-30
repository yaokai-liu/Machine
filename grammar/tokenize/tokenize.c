/* License
 *
 * ${PROJ_DESCRIPTION}
 * Copyright (C) 2025 Yaokai Liu
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
 *
 * Project Name: machine
 * Module Name: grammar/tokenize
 * Filename: tokenizer.c
 * Creator: Yaokai Liu
 * Create Date: 2025-05-10
 * Copyright (c) 2025 Yaokai Liu. All rights reserved.
 **/

#include "tokenize.h"
#include "enum.h"
#include "parse.h"
#include "tokenizer.h"
#include <string.h>
uint32_t Tokenizer_lex_token_from_source(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_concat_to_token(
    Tokenizer *tokenizer, const Concat *concat, Token *token, ErrInfo *err_info
);
uint32_t Tokenizer_parse_and_store_macro_def(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_parse_and_enter_macro_call(
    Tokenizer *tokenizer, REFER(Macro) v_macro, Token *token, ErrInfo *err_info
);
uint32_t Tokenizer_next_in_src(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
uint32_t Tokenizer_next_in_frame(Tokenizer *tokenizer, Token *token, ErrInfo *err_info);
bool Tokenizer_end_of_frame(Tokenizer *tokenizer);
void Tokenizer_exit_frame(Tokenizer *tokenizer);

#define pText (tokenizer->src + tokenizer->cost)
inline uint32_t Tokenizer_lex_token_from_source(
    Tokenizer * const tokenizer, Token * const token, ErrInfo * const err_info
) {
  uint32_t cost = pass_space(pText, &tokenizer->lineno, &tokenizer->column);
  tokenizer->cost += cost;

  if (!*pText) {
    token->position[0].lineno = tokenizer->lineno;
    token->position[0].column = tokenizer->column;
    token->position[1].lineno = tokenizer->lineno;
    token->position[1].column = tokenizer->column;
    token->type = Machine_TOKEN_TERMINATOR;
    token->value = nullptr;
    return SUCCESS;
  }

  Terminal terminal = {};
  terminal.lineno = tokenizer->lineno;
  terminal.column = tokenizer->column;
  cost = single_tokenize(pText, &terminal, tokenizer->allocator);
  if (0 == cost) {
    err_info->pos[0].lineno = tokenizer->lineno;
    err_info->pos[1].column = tokenizer->column;
    err_info->pos[1] = err_info->pos[0];
    err_info->stage = COMPILER_LEX;
    err_info->token = Machine_TOKEN_BAD_TOKEN;
    err_info->code = ERROR_UNKNOWN_SYMBOL;
    return ERROR_UNKNOWN_SYMBOL;
  }
  tokenizer->column += cost;
  tokenizer->cost += cost;

  if (terminal.type == Machine_TOKEN_IDENTIFIER) {
    REFER(Identifier) ident = Trie_get(tokenizer->ident_trie, terminal.value);
    if (!ident) {
      Array_append(tokenizer->ident_array, terminal.value, terminal.length + 1);
      ident = Array_last_virt(tokenizer->ident_array) - terminal.length;
      Trie_set(tokenizer->ident_trie, terminal.value, ident);
    }
    tokenizer->allocator->free(terminal.value);
    terminal.value = ident;
  }

  terminal_to_token(token, &terminal);

  return SUCCESS;
}

inline uint32_t Tokenizer_concat_to_token(
    Tokenizer * const tokenizer, const Concat *concat, Token * const token, ErrInfo * const err_info
) {
  const Token *tokens = Array_first_real(concat);
  const uint32_t count = Array_length(concat);

  // push strings to ident_array
  Array *ident_array = Array_new(sizeof(char_t), Machine_TOKEN_IDENTIFIER, tokenizer->allocator);
  for (uint32_t i = 0; i < count; i++) {
    const Token *tp = &tokens[i];
    if (tokens[i].type == Machine_TOKEN_PLACE_HOLDER) {
      const uint32_t index = (uint32_t) (uint64_t) tokens[i].value;
      MacroArg *arg = Array_real_addr(tokenizer->frame.args, index);
      tp = arg->target;
    }
    if (tp->type != Machine_TOKEN_IDENTIFIER) {
      releasePrimeArray(ident_array);
      err_info->pos[0] = tp->position[0];
      err_info->pos[1] = tp->position[1];
      err_info->code = ERROR_UNEXPECTED_CONCAT_TOKEN;
      err_info->stage = COMPILER_MACRO;
      err_info->token = tp->type;
      return ERROR_UNEXPECTED_CONCAT_TOKEN;
    }
    const char_t *sym_str = Array_virt2real(tokenizer->ident_array, tp->value);
    const uint32_t sym_len = strlen(sym_str);
    Array_append(ident_array, sym_str, sym_len);
  }
  Array_append(ident_array, "\0", 1);

  const char_t *sym_str = Array_first_real(ident_array);
  REFER(Identifier) v_sym = Trie_get(tokenizer->ident_trie, sym_str);
  if (!v_sym) {
    // add an identifier record
    v_sym = ((char_t *) Array_last_virt(tokenizer->ident_array)) + 1;
    Array_concat(tokenizer->ident_array, ident_array);
    Trie_set(tokenizer->ident_trie, sym_str, v_sym);
  }

  releasePrimeArray(ident_array);
  token->type = Machine_TOKEN_IDENTIFIER;
  token->value = v_sym;
  return SUCCESS;
}

uint32_t
    Tokenizer_next_in_src(Tokenizer * const tokenizer, Token *token, ErrInfo * const err_info) {
  uint32_t result = ERROR_WHATEVER;
  do {
    result = Tokenizer_lex_token_from_source(tokenizer, token, err_info);
    if (result != SUCCESS) { return result; }
    while (token->type == Machine_TOKEN_MACRO) {
      result = Tokenizer_parse_and_store_macro_def(tokenizer, token, err_info);
      if (result != SUCCESS) { return result; }
      result = Tokenizer_lex_token_from_source(tokenizer, token, err_info);
      if (result != SUCCESS) { return result; }
    }
    while (token->type == Machine_TOKEN_IDENTIFIER) {
      REFER(Macro) v_macro = MacroContext_findMacro(tokenizer->context, token->value);
      if (!v_macro) { return SUCCESS; }
      result = Tokenizer_parse_and_enter_macro_call(tokenizer, v_macro, token, err_info);
      if (result != SUCCESS) { return result; }
      result = Tokenizer_next_in_frame(tokenizer, token, err_info);
      if (result == END_OF_MACRO_FRAME) { break; }
    }
  } while (result == END_OF_MACRO_FRAME);
  return result;
}

uint32_t
    Tokenizer_next_in_frame(Tokenizer * const tokenizer, Token *token, ErrInfo * const err_info) {
  uint32_t result = ERROR_WHATEVER;
  while (true) {
    Token *tp = nullptr;
    while (tokenizer->frame.tokens && !tp) {
      tp = Array_real_addr(tokenizer->frame.tokens, tokenizer->frame.index++);
      if (!tp && Tokenizer_end_of_frame(tokenizer)) { Tokenizer_exit_frame(tokenizer); }
    }
    if (!tp) { return END_OF_MACRO_FRAME; }

    if (tp->type == Machine_TOKEN_Concat) {
      const Concat *concat = Array_virt2real(tokenizer->frame.concatArray, tp->value);
      result = Tokenizer_concat_to_token(tokenizer, concat, token, err_info);
      if (result != SUCCESS) { return result; }
      tp = token;
    }

    if (tp->type == Machine_TOKEN_PLACE_HOLDER) {
      const uint32_t index = (uint32_t) (uint64_t) tp->value;
      if (!tokenizer->frame.args || index > Array_length(tokenizer->frame.args)) {
        err_info->pos[0] = tp->position[0];
        err_info->pos[1] = tp->position[1];
        err_info->code = ERROR_INDEX_OUT_OF_RANGE;
        err_info->stage = COMPILER_MACRO;
        err_info->token = index;
        return ERROR_INDEX_OUT_OF_RANGE;
      }
      const MacroArg *arg = Array_real_addr(tokenizer->frame.args, index);
      if (arg->type != Machine_TOKEN_Tokens) {
        tp = arg->target;
        *token = *tp;
        return SUCCESS;
      }

      Stack_push(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
      tokenizer->frame.position[0] = tp->position[0];
      tokenizer->frame.position[1] = tp->position[1];
      tokenizer->frame.tokens = arg->target;
      tokenizer->frame.args = nullptr;
      tokenizer->frame.index = 0;
      continue;
    }

    if (tp->type != Machine_TOKEN_IDENTIFIER) {
      *token = *tp;
      return SUCCESS;
    }

    REFER(Macro) v_macro = MacroContext_findMacro(tokenizer->context, tp->value);
    if (!v_macro) {
      *token = *tp;
      return SUCCESS;
    }

    *token = *tp;
    result = Tokenizer_parse_and_enter_macro_call(tokenizer, v_macro, token, err_info);
    if (result != SUCCESS) { return result; }
  }
}

uint32_t Tokenizer_parse_and_store_macro_def(
    Tokenizer * const tokenizer, Token * const token, ErrInfo * const err_info
) {
  return Tokenizer_parse(tokenizer, token, err_info);
}

uint32_t Tokenizer_parse_and_enter_macro_call(
    Tokenizer * const tokenizer, REFER(Macro) v_macro, Token * const token, ErrInfo * const err_info
) {
  uint32_t result = ERROR_WHATEVER;
  const Macro *macro = MacroContext_macroReal(tokenizer->context, v_macro);
  if (macro->params) {
    result = Tokenizer_parse(tokenizer, token, err_info);
    if (result != SUCCESS) { return result; }
  }
  Stack_push(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
  MacroContext_makeFrame(tokenizer->context, &tokenizer->frame, v_macro);
  tokenizer->frame.position[0] = token->position[0];
  tokenizer->frame.position[1] = token->position[1];
  return SUCCESS;
}

inline bool Tokenizer_end_of_frame(Tokenizer * const tokenizer) {
  return tokenizer->frame.tokens && tokenizer->frame.index >= Array_length(tokenizer->frame.tokens);
}

inline void Tokenizer_exit_frame(Tokenizer * const tokenizer) {
  while (Tokenizer_end_of_frame(tokenizer)) {
    if ((uint64_t) tokenizer->frame.args > Machine_TOKEN_MacroArgs) {
      Array_reset(tokenizer->frame.args, (destruct_t *) releaseMacroArg);
      Array_destroy(tokenizer->frame.args);
    }
    Stack_pop(tokenizer->framestack, &tokenizer->frame, sizeof(MacroCallFrame));
  }
}

uint32_t Tokenizer_next(Tokenizer * const tokenizer, Token *token, ErrInfo * const err_info) {
  uint32_t result = ERROR_WHATEVER;
  if (tokenizer->frame.tokens) {
    result = Tokenizer_next_in_frame(tokenizer, token, err_info);
    if (result == END_OF_MACRO_FRAME) {
      result = Tokenizer_next_in_src(tokenizer, token, err_info);
      if (result != SUCCESS) { return result; }
    }
  } else {
    result = Tokenizer_next_in_src(tokenizer, token, err_info);
  }
  return result;
}

inline uint32_t Tokenizer_macro_next(
    Tokenizer * const tokenizer, Token * const token, ErrInfo * const err_info
) {
  if (tokenizer->context->end_parse) {
    token->position[0].lineno = tokenizer->lineno;
    token->position[0].column = tokenizer->column;
    token->position[1].lineno = tokenizer->lineno;
    token->position[1].column = tokenizer->column;
    token->type = Machine_TOKEN_TERMINATOR;
    token->value = nullptr;
    return SUCCESS;
  }
  uint32_t result = ERROR_WHATEVER;
  if (tokenizer->frame.tokens) {
    result = Tokenizer_next_in_frame(tokenizer, token, err_info);
    if (result == END_OF_MACRO_FRAME) {
      result = Tokenizer_lex_token_from_source(tokenizer, token, err_info);
      if (result != SUCCESS) { return result; }
    }
  } else {
    result = Tokenizer_lex_token_from_source(tokenizer, token, err_info);
  }
  if (result != SUCCESS) { return result; }

  switch (token->type) {
    case Machine_TOKEN_MACRO:
    case Machine_TOKEN_CONCAT: {
      return result;
    }
    case Machine_TOKEN_LEFT_BRACKET: {
      if (tokenizer->context->depth++ == 0) { return result; }
      break;
    }
    case Machine_TOKEN_RIGHT_BRACKET: {
      if (--tokenizer->context->depth == 0) { return result; }
      break;
    }
    case Machine_TOKEN_COMMA:
    case Machine_TOKEN_NUMBER:
    case Machine_TOKEN_LEFT_PAREN:
    case Machine_TOKEN_RIGHT_PAREN:
    case Machine_TOKEN_IDENTIFIER: {
      if (!tokenizer->context->in_macro) { return result; }
      break;
    }
    default: {
    }
  }

  Token *tp = tokenizer->allocator->calloc(1, sizeof(Token));
  *tp = *token;

  token->type = Machine_TOKEN_TOKEN;
  token->value = tp;
  return result;
}
