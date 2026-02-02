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
slash:
  out += encode_slash(out);
  COMP_DISPATCH();
identifier:
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode_identifier(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(CONT_IDENTIFIER);
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
start_comment:
less_than:
equals:
greater_than:
question:
at:
backslash:
circumflex:
grave:
lbrace:
vbar:
rbrace:
tilde:
unknown:
  out += encode_unknown(out, it - tok_start);
  COMP_DISPATCH();
eof:
  out += encode_eof(out);
  return out - out_buffer;
}
// constexpr value_type lex_cr() {
//   assert(*start == '\r');
//   if (*it_ == '\n') {
//     ++it_;
//     return make_lexeme(token_kind::newline_crlf, start);
//   }
//   return make_lexeme(token_kind::newline_cr, start);
// }

// constexpr value_type lex_unquote() {
//   assert(*start == ',');
//   if (*it_ == '@') {
//     ++it_;
//     return make_lexeme(token_kind::unquote_splicing, start);
//   }

//   return make_lexeme(token_kind::unquote, start);
// }

// constexpr value_type lex_syntax(const char* start) {
//   assert(*start == '#');
//   switch (*it_) {
//   case '\'':
//     ++it_;
//     return make_lexeme(token_kind::syntax, start);
//   case '`':
//     ++it_;
//     return make_lexeme(token_kind::quasisyntax, start);
//   case ',':
//     ++it_;
//     if (*it_ == '@') {
//       ++it_;
//       return make_lexeme(token_kind::unsyntax_splicing, start);
//     }
//     return make_lexeme(token_kind::unsyntax, start);
//   }

//   return make_lexeme(token_kind::unknown, start);
// }

// constexpr value_type lex_string(const char* start) {
//   assert(*start == '"');

//   for (auto c = *it_; c != '\0'; c = *++it_) {

//     if (c == '"') {
//       ++it_;
//       return make_lexeme(token_kind::string_lit, start);
//     }
//   }

//   return make_lexeme(token_kind::unterminated_string_lit, start);
// }

// constexpr value_type lex_number(const char* start) {
//   assert(detail::is_digit(*start));

//   skip_while(detail::is_digit<char>);

//   if (*it_ == '.') {
//     ++it_;
//     skip_while(detail::is_digit<char>);
//     if (!detail::is_delimiter(*it_)) {
//       // identifier now
//       skip_until(detail::is_delimiter<char>);
//       return make_lexeme(token_kind::identifier, start);
//     }
//     return make_lexeme(token_kind::decimal_lit, start);
//   } else {
//     if (!detail::is_delimiter(*it_)) {
//       skip_until(detail::is_delimiter<char>);
//       return make_lexeme(token_kind::identifier, start);
//     }

//     return make_lexeme(token_kind::integer_lit, start);
//   }
// }

// constexpr value_type lex_identifier(const char* start) {
//   skip_until(detail::is_delimiter<char>);
//   return make_lexeme(token_kind::identifier, start);
// }

// // skips the pred's return while pred(*it_) != 0
// template <typename P> constexpr void skip_n_while(P pred) {
//   for (;;) {
//     auto skip = pred(*it_);
//     if (skip == 0)
//       return;
//     it_ += skip;
//   }
// }

// // skips characters while pred(*it_) is fulfilled
// template <typename P> constexpr void skip_while(P pred) {
//   skip_n_while(pred);
// }

// template <typename P> constexpr void skip_until(P pred) {
//   skip_while(std::not_fn<P>(std::move(pred)));
// }

// template <> struct fmt::formatter<ely::stx::token_kind> {
//   constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin();
//   } template <typename Ctx> constexpr auto format(ely::stx::token_kind k,
//   Ctx& ctx) const {
//     return fmt::format_to(ctx.out(), "{}", ely::stx::token_kind_name(k));
//   }
// };