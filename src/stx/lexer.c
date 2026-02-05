#include "cont.h"
#include "encode.h"

#include <stddef.h>
#include <stdint.h>

static inline bool is_digit(char c) { return '0' <= c && c <= '9'; }

static inline bool is_delimiter(char c) {
  switch (c) {
  case ' ':
  case '\t':
  case '\r':
  case '\n':
  case '\0':
  case ';':
  case '/':
  case '(':
  case ')':
  case '[':
  case ']':
  case '{':
  case '}':
    return true;
  default:
    return false;
  }
}

size_t ely_lex(const char* src, size_t src_len, uint8_t* out_buffer,
               size_t out_buffer_len, uint8_t cont_id) {
  // need at least 4 bytes for possible encodings
  if (out_buffer_len < 4) {
    return 0;
  }
  const char* it = src;
  const char* end = src + src_len;
  const char* tok_start = it;
  uint8_t* out = out_buffer;

  static const void* cont_table[] = {
      [CONT_START] = &&start,
      [CONT_WHITESPACE] = &&whitespace,
      [CONT_TAB] = &&tab,
      [CONT_NEWLINE_CR] = &&newline_cr,
      [CONT_IDENTIFIER] = &&identifier,
      [CONT_DECIMAL_LIT] = &&decimal,
      [CONT_INTEGER_LIT] = &&number,
      [CONT_STRING_LIT] = &&string_lit,
      [CONT_LINE_COMMENT] = &&start_comment,
      [CONT_LINE_COMMENT_CR] = &&line_comment_cr,
      [CONT_UNICODE4] = &&unicode4,
      [CONT_UNICODE3] = &&unicode3,
      [CONT_UNICODE2] = &&unicode2,
  };

  static const void* dispatch[256] = {
      [0 ... 255] = &&unknown,
      ['\0'] = &&eof,
      ['\t'] = &&tab,
      ['\n'] = &&newline_lf,
      ['\r'] = &&newline_cr,
      [' '] = &&whitespace,
      ['!'] = &&exclamation,
      ['"'] = &&string_lit,
      ['#'] = &&number_sign,
      ['$'] = &&dollar,
      ['%'] = &&percent,
      ['&'] = &&ampersand,
      ['\''] = &&single_quote,
      ['('] = &&lparen,
      [')'] = &&rparen,
      ['*'] = &&asterisk,
      ['+'] = &&plus,
      [','] = &&comma,
      ['-'] = &&identifier,
      ['.'] = &&period,
      ['/'] = &&slash,
      ['0' ... '9'] = &&number,
      [':'] = &&colon,
      [';'] = &&start_comment,
      ['<'] = &&less_than,
      ['='] = &&equals,
      ['>'] = &&greater_than,
      ['?'] = &&question,
      ['@'] = &&at,
      ['A' ... 'Z'] = &&identifier,
      ['['] = &&lbracket,
      ['\\'] = &&backslash,
      // for some reason vscode syntax highlighting messes up on ]
      [']'] = &&rbracket,
      ['^'] = &&circumflex,
      ['_'] = &&identifier,
      ['`'] = &&grave,
      ['a' ... 'z'] = &&identifier,
      ['{'] = &&lbrace,
      ['|'] = &&vbar,
      ['}'] = &&rbrace,
      ['~'] = &&tilde,
      [0b11000000 ... 0b11011111] = &&unicode2,
      [0b11100000 ... 0b11101111] = &&unicode3,
      [0b11110000 ... 0b11110111] = &&unicode4,
  };

#define DO_SPILL(id)                                                           \
  do {                                                                         \
    out += encode_spill(out, it - tok_start, id);                              \
    return out - out_buffer;                                                   \
  } while (false)

#define COMP_DISPATCH()                                                        \
  do {                                                                         \
    tok_start = it;                                                            \
    if (it == end) {                                                           \
      DO_SPILL(CONT_START);                                                    \
    }                                                                          \
    if ((out_buffer + out_buffer_len - out) < 4) {                             \
      out += encode_buffer_full(out);                                          \
      return out - out_buffer;                                                 \
    }                                                                          \
    goto* dispatch[*it++];                                                     \
  } while (false)

  goto* cont_table[cont_id];

start:
  COMP_DISPATCH();

whitespace:
  for (; it != end; ++it) {
    if (*it != ' ') {
      out += encode_whitespace(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_WHITESPACE);
tab:
  for (; it != end; ++it) {
    if (*it != '\t') {
      out += encode_tab(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_TAB);
newline_lf:
  out += encode_newline_lf(out);
  COMP_DISPATCH();
newline_cr:
  if (it == end) {
    DO_SPILL(CONT_NEWLINE_CR);
  }
  if (*it == '\n') {
    ++it;
    out += encode_newline_crlf(out);
  } else {
    out += encode_newline_cr(out);
  }
  COMP_DISPATCH();
lparen:
  out += encode_lparen(out);
  COMP_DISPATCH();
rparen:
  out += encode_rparen(out);
  COMP_DISPATCH();
lbracket:
  out += encode_lbracket(out);
  COMP_DISPATCH();
rbracket:
  out += encode_rbracket(out);
  COMP_DISPATCH();
lbrace:
  out += encode_lbrace(out);
  COMP_DISPATCH();
rbrace:
  out += encode_rbrace(out);
  COMP_DISPATCH();
slash:
  out += encode_slash(out);
  COMP_DISPATCH();
number:
  for (; it != end; ++it) {
    if (*it == '.') {
      ++it;
      goto*&& decimal;
    } else if (is_delimiter(*it)) {
      out += encode_integer_lit(out, it - tok_start);
      COMP_DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      goto*&& identifier;
    }
  }
decimal:
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode_decimal_lit(out, it - tok_start);
      COMP_DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      // identifier now
      goto*&& identifier;
    }
  }

  DO_SPILL(CONT_DECIMAL_LIT);
string_lit:
  // skipped starting "
  for (; it != end; ++it) {
    // TODO: handle escapes
    if (*it == '"') {
      ++it;
      out += encode_string_lit(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_STRING_LIT);
start_comment:
  for (; it != end; ++it) {
    if (*it == '\n') {
      ++it;
      out += encode_line_comment(out, it - tok_start);
      COMP_DISPATCH();
    } else if (*it == '\r') {
      ++it;
      if (it == end) {
        DO_SPILL(CONT_LINE_COMMENT_CR);
      }
    line_comment_cr:
      if (*it == '\n') {
        ++it;
      }
      out += encode_line_comment(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_LINE_COMMENT);
unicode4:
  if (it == end) {
    DO_SPILL(CONT_UNICODE4);
  }
  ++it;
  // fallthrough to unicode3
unicode3:
  if (it == end) {
    DO_SPILL(CONT_UNICODE3);
  }
  ++it;
  // fallthrough to unicode2
unicode2:
  if (it == end) {
    DO_SPILL(CONT_UNICODE2);
  }
  ++it;
  // fallthrough to unknown
exclamation:
number_sign:
dollar:
percent:
ampersand:
single_quote:
asterisk:
plus:
comma:
period:
colon:
less_than:
equals:
greater_than:
question:
at:
backslash:
circumflex:
grave:
vbar:
tilde:
identifier:
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode_identifier(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_IDENTIFIER);
unknown:
  out += encode_unknown(out, it - tok_start);
  COMP_DISPATCH();
eof:
  out += encode_eof(out);
  return out - out_buffer;
}
