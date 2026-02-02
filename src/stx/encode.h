#pragma once

#include <stddef.h>

#include "ely/config.h"
#include "ely/stx/tokens.h"

#ifdef __cplusplus
extern "C" {
#endif

ELY_ALWAYS_INLINE static size_t encode_whitespace(uint8_t* out, size_t num) {
  *out++ = ELY_TOKEN_WHITESPACE;
  *out = num;
  return 2;
}

ELY_ALWAYS_INLINE static size_t encode_tab(uint8_t* out, size_t num) {
  *out++ = ELY_TOKEN_TAB;
  *out = num;
  return 2;
}

ELY_ALWAYS_INLINE static size_t encode_newline_crlf(uint8_t* out) {
  *out = ELY_TOKEN_NEWLINE_CRLF;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_newline_cr(uint8_t* out) {
  *out = ELY_TOKEN_NEWLINE_CR;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_newline_lf(uint8_t* out) {
  *out = ELY_TOKEN_NEWLINE_LF;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_line_comment(uint8_t* out, size_t num) {
  *out++ = ELY_TOKEN_LINE_COMMENT;
  *out = num;
  return 2;
}

ELY_ALWAYS_INLINE static size_t encode_block_comment(uint8_t* out, size_t num,
                                                     size_t newlines) {
  *out++ = ELY_TOKEN_BLOCK_COMMENT;
  *out++ = num;
  *out = newlines;
  return 3;
}

ELY_ALWAYS_INLINE static size_t encode_lparen(uint8_t* out) {
  *out = ELY_TOKEN_LPAREN;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_rparen(uint8_t* out) {
  *out = ELY_TOKEN_RPAREN;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_lbracket(uint8_t* out) {
  *out = ELY_TOKEN_LBRACKET;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_rbracket(uint8_t* out) {
  *out = ELY_TOKEN_RBRACKET;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_lbrace(uint8_t* out) {
  *out = ELY_TOKEN_LBRACE;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_rbrace(uint8_t* out) {
  *out = ELY_TOKEN_RBRACE;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_slash(uint8_t* out) {
  *out = ELY_TOKEN_SLASH;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_identifier(uint8_t* out, size_t num) {
  *out++ = ELY_TOKEN_IDENTIFIER;
  *out = num;
  return 2;
}

ELY_ALWAYS_INLINE static size_t encode_decimal_lit(uint8_t* out, size_t num) {
  *out++ = ELY_TOKEN_DECIMAL_LIT;
  *out = num;
  return 2;
}

ELY_ALWAYS_INLINE static size_t encode_unknown(uint8_t* out) {
  *out = ELY_TOKEN_UNKNOWN;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_spill(uint8_t* out, size_t num,
                                             size_t cont_id) {
  // encodes in reverse order as the final status is always read from the back
  *out++ = (uint8_t)num;
  *out++ = (uint8_t)cont_id;
  *out = ELY_TOKEN_SPILL;
  return 3;
}

ELY_ALWAYS_INLINE static size_t encode_eof(uint8_t* out) {
  *out = ELY_TOKEN_EOF;
  return 1;
}

ELY_ALWAYS_INLINE static size_t encode_buffer_full(uint8_t* out) {
  *out = ELY_TOKEN_BUFFER_FULL;
  return 1;
}

#ifdef __cplusplus
} // extern "C"
#endif
