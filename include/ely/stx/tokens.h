#pragma once

#include <assert.h>
#include <stdint.h>

typedef enum ely_token_kind : uint8_t {
#define TOKEN(x) ELY_TOKEN_##x,
#define TOKEN2(x, c) ELY_TOKEN_##x = c,
#include "tokens.def"
#undef TOKEN
#undef TOKEN2
} ely_token_kind;

static inline bool ely_token_is_newline(ely_token_kind tk) {
  switch (tk) {
  case ELY_TOKEN_NEWLINE_LF:
  case ELY_TOKEN_NEWLINE_CR:
  case ELY_TOKEN_NEWLINE_CRLF:
    return true;
  default:
    return false;
  }
}

static inline bool ely_token_is_atmosphere(ely_token_kind tk) {
  switch (tk) {
  case ELY_TOKEN_WHITESPACE:
  case ELY_TOKEN_TAB:
  case ELY_TOKEN_NEWLINE_CR:
  case ELY_TOKEN_NEWLINE_LF:
  case ELY_TOKEN_NEWLINE_CRLF:
  case ELY_TOKEN_LINE_COMMENT:
  case ELY_TOKEN_BLOCK_COMMENT:
    return true;
  default:
    return false;
  }
}
