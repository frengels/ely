#include <array>

#include "ely/stx/cont.hpp"
#include "ely/stx/encode.hpp"

#include <span>
#include <string_view>

namespace ely {
namespace stx {
namespace {
template <typename CharT> constexpr bool is_digit(CharT c) {
  return '0' <= c && c <= '9';
}

template <typename CharT> constexpr bool is_delimiter(CharT c) {
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
} // namespace

std::size_t lex(std::string_view src, std::span<uint8_t> out_buffer,
                uint8_t cont_id) {
  // need at least 4 bytes for possible encodings
  if (out_buffer.size() < 4) {
    return 0;
  }
  const char* it = src.data();
  const char* end = src.data() + src.size();
  const char* tok_start = it;
  uint8_t* out = out_buffer.data();

  // surprised that designated initializers for static arrays are working here.
  static constexpr void* cont_table[] = {
      [std::to_underlying(cont::start)] = &&start,
      [std::to_underlying(cont::whitespace)] = &&whitespace,
      [std::to_underlying(cont::tab)] = &&tab,
      [std::to_underlying(cont::newline_cr)] = &&newline_cr,
      [std::to_underlying(cont::identifier)] = &&identifier,
      [std::to_underlying(cont::decimal_lit)] = &&decimal,
      [std::to_underlying(cont::integer_lit)] = &&number,
      [std::to_underlying(cont::string_lit)] = &&string_lit,
      [std::to_underlying(cont::line_comment)] = &&start_comment,
      [std::to_underlying(cont::line_comment_cr)] = &&line_comment_cr,
      [std::to_underlying(cont::unicode4)] = &&unicode4,
      [std::to_underlying(cont::unicode3)] = &&unicode3,
      [std::to_underlying(cont::unicode2)] = &&unicode2,
  };

  // this madness is required because C++ doesn't allow designated initializers
  // for static arrays.
  constexpr void* eof = &&eof;
  constexpr void* newline_lf = &&newline_lf;
  constexpr void* newline_cr = &&newline_cr;
  constexpr void* whitespace = &&whitespace;
  constexpr void* tab = &&tab;
  constexpr void* exclamation = &&exclamation;
  constexpr void* string_lit = &&string_lit;
  constexpr void* number_sign = &&number_sign;
  constexpr void* dollar = &&dollar;
  constexpr void* percent = &&percent;
  constexpr void* ampersand = &&ampersand;
  constexpr void* single_quote = &&single_quote;
  constexpr void* lparen = &&lparen;
  constexpr void* rparen = &&rparen;
  constexpr void* lbracket = &&lbracket;
  constexpr void* rbracket = &&rbracket;
  constexpr void* lbrace = &&lbrace;
  constexpr void* rbrace = &&rbrace;
  constexpr void* asterisk = &&asterisk;
  constexpr void* plus = &&plus;
  constexpr void* comma = &&comma;
  constexpr void* period = &&period;
  constexpr void* slash = &&slash;
  constexpr void* colon = &&colon;
  constexpr void* less_than = &&less_than;
  constexpr void* equals = &&equals;
  constexpr void* greater_than = &&greater_than;
  constexpr void* question = &&question;
  constexpr void* at = &&at;
  constexpr void* unknown = &&unknown;
  constexpr void* identifier = &&identifier;
  constexpr void* number = &&number;
  constexpr void* decimal = &&decimal;
  constexpr void* start_comment = &&start_comment;
  constexpr void* backslash = &&backslash;
  constexpr void* circumflex = &&circumflex;
  constexpr void* grave = &&grave;
  constexpr void* vbar = &&vbar;

  static constexpr std::array<const void*, 256> dispatch =
      [](void* unknown, void* unicode2, void* unicode3, void* unicode4) {
        // assign all the ranges because c++ doesn't support the [a ... b] GNU
        // extension for static arrays, and we want to keep the dispatch table
        // constexpr. We also want to keep the explicitly assigned entries in
        // place, so we can't just default to unknown and then overwrite the
        // assigned ones.
        std::array<const void*, 256> res{};
        for (std::size_t i = 0; i != 256; ++i) {

          res[i] = unknown;
        }
        res['\0'] = eof;
        res['\t'] = tab;
        res['\n'] = newline_lf;
        res['\r'] = newline_cr;
        res[' '] = whitespace;
        res['!'] = exclamation;
        res['"'] = string_lit;
        res['#'] = number_sign;
        res['$'] = dollar;
        res['%'] = percent;
        res['&'] = ampersand;
        res['\''] = single_quote;
        res['('] = lparen;
        res[')'] = rparen;
        res['*'] = asterisk;
        res['+'] = plus;
        res[','] = comma;
        res['-'] = identifier; // could be minus or start of identifier
        res['.'] = period;     // could be dot or start of decimal literal
        res['/'] = slash;
        res[':'] = colon;
        res[';'] = start_comment;
        res['<'] = less_than;
        res['='] = equals;
        res['>'] = greater_than;
        res['?'] = question;
        res['@'] = at;
        res['['] = lbracket;
        res['\\'] = backslash;
        res[']'] = rbracket;
        res['^'] = circumflex;
        res['_'] = identifier;
        res['`'] = grave;
        res['{'] = lbrace;
        res['|'] = vbar;
        res['}'] = rbrace;
        // assign the ranges for identifiers and unicode characters

        for (char c = '0'; c <= '9'; ++c) {
          res[c] = number;
        }
        for (char c = 'a'; c <= 'z'; ++c) {
          res[c] = identifier;
        }
        for (char c = 'A'; c <= 'Z'; ++c) {
          res[c] = identifier;
        }
        for (std::size_t i = 0b11000000; i <= 0b11011111; ++i) {
          res[i] = unicode2;
        }
        for (std::size_t i = 0b11100000; i <= 0b11101111; ++i) {
          res[i] = unicode3;
        }
        for (std::size_t i = 0b11110000; i <= 0b11110111; ++i) {
          res[i] = unicode4;
        }

        return res;
      }(&&unknown, &&unicode2, &&unicode3, &&unicode4);

#define DO_SPILL(id)                                                           \
  do {                                                                         \
    out += encode<token_kind::spill>(out, it - tok_start,                      \
                                     static_cast<std::uint8_t>(id));           \
    return out - out_buffer.data();                                            \
  } while (false)

#define COMP_DISPATCH()                                                        \
  do {                                                                         \
    tok_start = it;                                                            \
    if (it == end) {                                                           \
      DO_SPILL(cont::start);                                                   \
    }                                                                          \
    if ((out_buffer.data() + out_buffer.size() - out) < 4) {                   \
      out += encode<token_kind::buffer_full>(out);                             \
      return out - out_buffer.data();                                          \
    }                                                                          \
    goto* dispatch[*it++];                                                     \
  } while (false)

  goto* cont_table[cont_id];

start:
  COMP_DISPATCH();

whitespace:
  for (; it != end; ++it) {
    if (*it != ' ') {
      out += encode<token_kind::whitespace>(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(cont::whitespace);
tab:
  for (; it != end; ++it) {
    if (*it != '\t') {
      out += encode<token_kind::tab>(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(cont::tab);
newline_lf:
  out += encode<token_kind::newline_lf>(out);
  COMP_DISPATCH();
newline_cr:
  if (it == end) {
    DO_SPILL(cont::newline_cr);
  }
  if (*it == '\n') {
    ++it;
    out += encode<token_kind::newline_crlf>(out);
  } else {
    out += encode<token_kind::newline_cr>(out);
  }
  COMP_DISPATCH();
lparen:
  out += encode<token_kind::lparen>(out);
  COMP_DISPATCH();
rparen:
  out += encode<token_kind::rparen>(out);
  COMP_DISPATCH();
lbracket:
  out += encode<token_kind::lbracket>(out);
  COMP_DISPATCH();
rbracket:
  out += encode<token_kind::rbracket>(out);
  COMP_DISPATCH();
lbrace:
  out += encode<token_kind::lbrace>(out);
  COMP_DISPATCH();
rbrace:
  out += encode<token_kind::rbrace>(out);
  COMP_DISPATCH();
slash:
  out += encode<token_kind::slash>(out);
  COMP_DISPATCH();
number:
  for (; it != end; ++it) {
    if (*it == '.') {
      ++it;
      goto*&& decimal;
    } else if (is_delimiter(*it)) {
      out += encode<token_kind::integer_lit>(out, it - tok_start);
      COMP_DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      goto*&& identifier;
    }
  }
  DO_SPILL(cont::integer_lit);
decimal:
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode<token_kind::decimal_lit>(out, it - tok_start);
      COMP_DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      // identifier now
      goto*&& identifier;
    }
  }

  DO_SPILL(cont::decimal_lit);
string_lit:
  // skipped starting "
  for (; it != end; ++it) {
    // TODO: handle escapes
    if (*it == '"') {
      ++it;
      out += encode<token_kind::string_lit>(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(cont::string_lit);
start_comment:
  for (; it != end; ++it) {
    if (*it == '\n') {
      ++it;
      out += encode<token_kind::line_comment>(out, it - tok_start);
      COMP_DISPATCH();
    } else if (*it == '\r') {
      ++it;
      if (it == end) {
        DO_SPILL(cont::line_comment_cr);
      }
    line_comment_cr:
      if (*it == '\n') {
        ++it;
      }
      out += encode<token_kind::line_comment>(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(cont::line_comment);
unicode4:
  if (it == end) {
    DO_SPILL(cont::unicode4);
  }
  ++it;
// fallthrough to unicode3
unicode3:
  if (it == end) {
    DO_SPILL(cont::unicode3);
  }
  ++it;
// fallthrough to unicode2
unicode2:
  if (it == end) {
    DO_SPILL(cont::unicode2);
  }
  ++it;
// fallthrough to unknown
exclamation:
number_sign:
single_quote:

comma:
colon:

question:
backslash:
grave:
vbar:

at:
dollar:
percent:
ampersand:
period:
circumflex:
less_than:
equals:
greater_than:
tilde:
asterisk:
plus:
identifier:
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode<token_kind::identifier>(out, it - tok_start);
      COMP_DISPATCH();
    }
  }
  DO_SPILL(cont::identifier);
unknown:
  out += encode<token_kind::unknown>(out, it - tok_start);
  COMP_DISPATCH();
eof:
  out += encode<token_kind::eof>(out);
  return out - out_buffer.data();
}
} // namespace stx
} // namespace ely
