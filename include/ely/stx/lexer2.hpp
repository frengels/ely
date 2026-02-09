#pragma once

#include <array>
#include <cstdint>
#include <span>
#include <string_view>
#include <utility>

#include "ely/config.h"
#include "ely/dbg.hpp"

#include "ely/stx/cont.hpp"
#include "ely/stx/encode.hpp"
#include "ely/stx/tokens.hpp"

namespace ely {
namespace stx {
inline namespace lexer2 {
namespace {
template <typename CharT> ELY_ALWAYS_INLINE constexpr bool is_digit(CharT c) {
  return '0' <= c && c <= '9';
}

template <typename CharT>
ELY_ALWAYS_INLINE constexpr bool is_delimiter(CharT c) {
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

using fn_type = std::size_t(ELY_PRESERVE_NONE*)(const char*, const char*,
                                                const char*,
                                                const std::uint8_t*,
                                                const std::uint8_t*,
                                                std::uint8_t*);

constexpr std::size_t ELY_PRESERVE_NONE lex_unknown(const char*, const char*,
                                                    const char*,
                                                    const std::uint8_t*,
                                                    const std::uint8_t*,
                                                    std::uint8_t*);
constexpr std::size_t ELY_PRESERVE_NONE lex_eof(const char*, const char*,
                                                const char*,
                                                const std::uint8_t*,
                                                const std::uint8_t*,
                                                std::uint8_t*);
constexpr std::size_t ELY_PRESERVE_NONE lex_whitespace(const char*, const char*,
                                                       const char*,
                                                       const std::uint8_t*,
                                                       const std::uint8_t*,
                                                       std::uint8_t*);
constexpr std::size_t ELY_PRESERVE_NONE lex_tab(const char*, const char*,
                                                const char*,
                                                const std::uint8_t*,
                                                const std::uint8_t*,
                                                std::uint8_t*);

constexpr std::size_t ELY_PRESERVE_NONE lex_identifier(const char*, const char*,
                                                       const char*,
                                                       const std::uint8_t*,
                                                       const std::uint8_t*,
                                                       std::uint8_t*);
constexpr std::size_t ELY_PRESERVE_NONE lex_number(const char*, const char*,
                                                   const char*,
                                                   const std::uint8_t*,
                                                   const std::uint8_t*,
                                                   std::uint8_t*);
constexpr std::size_t ELY_PRESERVE_NONE lex_decimal(const char*, const char*,
                                                    const char*,
                                                    const std::uint8_t*,
                                                    const std::uint8_t*,
                                                    std::uint8_t* out);
constexpr std::size_t ELY_PRESERVE_NONE lex_string(const char*, const char*,
                                                   const char*,
                                                   const std::uint8_t*,
                                                   const std::uint8_t*,
                                                   std::uint8_t* out);
constexpr std::size_t ELY_PRESERVE_NONE lex_start(const char* it,
                                                  const char* end,
                                                  const char* tok_start,
                                                  const std::uint8_t* out_start,
                                                  const std::uint8_t* out_end,
                                                  std::uint8_t* out);
constexpr std::size_t ELY_PRESERVE_NONE
lex_newline_lf(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out);
constexpr std::size_t ELY_PRESERVE_NONE
lex_newline_cr(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out);

template <char C>
constexpr std::size_t ELY_PRESERVE_NONE lex_paren(const char* it,
                                                  const char* end,
                                                  const char* tok_start,
                                                  const std::uint8_t* out_start,
                                                  const std::uint8_t* out_end,
                                                  std::uint8_t* out);

constexpr std::size_t ELY_PRESERVE_NONE
lex_unreachable(const char*, const char*, const char*, const std::uint8_t*,
                const std::uint8_t*, std::uint8_t*) {
  ELY_UNIMPLEMENTED("This lexer has not yet been implemented");
}

inline constexpr auto cont_table = [] {
  using enum cont;
  std::array<fn_type, static_cast<std::size_t>(last) + 1> res{};
  res[std::to_underlying(start)] = &lex_start;
  res[std::to_underlying(whitespace)] = &lex_whitespace;
  res[std::to_underlying(tab)] = &lex_tab;
  res[std::to_underlying(newline_cr)] = &lex_newline_cr;
  res[std::to_underlying(identifier)] = &lex_identifier;
  res[std::to_underlying(decimal_lit)] = &lex_decimal;
  res[std::to_underlying(integer_lit)] = &lex_number;
  res[std::to_underlying(string_lit)] = &lex_string;
  res[std::to_underlying(line_comment)] = &lex_unreachable;
  res[std::to_underlying(line_comment_cr)] = &lex_unreachable;
  res[std::to_underlying(unicode4)] = &lex_unreachable;
  res[std::to_underlying(unicode3)] = &lex_unreachable;
  res[std::to_underlying(unicode2)] = &lex_unreachable;
  return res;
}();

inline constexpr auto jump_table = [] {
  std::array<fn_type, 256> tbl{};

  tbl['\0'] = &lex_eof;
  tbl[' '] = &lex_whitespace;
  tbl['\t'] = &lex_tab;
  tbl['-'] = &lex_identifier;
  tbl['_'] = &lex_identifier;
  tbl['('] = &lex_paren<'('>;
  tbl[')'] = &lex_paren<')'>;
  tbl['['] = &lex_paren<'['>;
  tbl[']'] = &lex_paren<']'>;
  tbl['{'] = &lex_paren<'{'>;
  tbl['}'] = &lex_paren<'}'>;
  tbl['"'] = &lex_string;
  tbl['\r'] = &lex_newline_cr;
  tbl['\n'] = &lex_newline_lf;

  for (auto c = 'a'; c <= 'z'; ++c) {
    tbl[c] = &lex_identifier;
  }

  for (auto c = 'A'; c <= 'Z'; ++c) {
    tbl[c] = &lex_identifier;
  }

  for (auto c = '0'; c <= '9'; ++c) {
    tbl[c] = &lex_number;
  }

  return tbl;
}();

#define DISPATCH()                                                             \
  do {                                                                         \
    tok_start = it;                                                            \
    if (it == end) {                                                           \
      out += encode<token_kind::spill>(out, it - tok_start, cont::start);      \
      return out - out_start;                                                  \
    }                                                                          \
    if ((out + 4) >= out_end) {                                                \
      out += encode<token_kind::buffer_full>(out);                             \
      return out - out_start;                                                  \
    }                                                                          \
    ELY_MUSTTAIL return jump_table[*it](it + 1, end, it, out_start, out_end,   \
                                        out);                                  \
  } while (0)

template <cont ContID>
ELY_COLD ELY_NOINLINE constexpr std::size_t ELY_PRESERVE_NONE
write_spill(const char* it, const char* end, const char* tok_start,
            const std::uint8_t* out_start, const std::uint8_t* out_end,
            std::uint8_t* out) {
  out += encode<token_kind::spill>(out, it - tok_start, ContID);
  return out - out_start;
}

ELY_COLD constexpr std::size_t ELY_PRESERVE_NONE
lex_unknown(const char* it, const char* end, const char* tok_start,
            const std::uint8_t* out_start, const std::uint8_t* out_end,
            std::uint8_t* out) {
  out += encode<token_kind::unknown>(out, it - tok_start);
  DISPATCH();
}

constexpr std::size_t ELY_PRESERVE_NONE lex_eof(const char* it, const char* end,
                                                const char* tok_start,
                                                const std::uint8_t* out_start,
                                                const std::uint8_t* out_end,
                                                std::uint8_t* out) {
  out += encode<token_kind::eof>(out);
  return out - out_start;
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_whitespace(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out) {
  for (; it != end; ++it) {
    if (*it != ' ') {
      out += encode<token_kind::whitespace>(out, it - tok_start);
      DISPATCH();
    }
  }
  ELY_MUSTTAIL return write_spill<cont::whitespace>(it, end, tok_start,
                                                    out_start, out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE lex_tab(const char* it, const char* end,
                                                const char* tok_start,
                                                const std::uint8_t* out_start,
                                                const std::uint8_t* out_end,
                                                std::uint8_t* out) {
  for (; it != end; ++it) {
    if (*it != '\t') {
      out += encode<token_kind::tab>(out, it - tok_start);
      DISPATCH();
    }
  }
  ELY_MUSTTAIL return write_spill<cont::tab>(it, end, tok_start, out_start,
                                             out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_identifier(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out) {
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode<token_kind::identifier>(out, it - tok_start);
      DISPATCH();
    }
  }
  ELY_MUSTTAIL return write_spill<cont::identifier>(it, end, tok_start,
                                                    out_start, out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_number(const char* it, const char* end, const char* tok_start,
           const std::uint8_t* out_start, const std::uint8_t* out_end,
           std::uint8_t* out) {
  for (; it != end; ++it) {
    if (*it == '.') {
      ++it;
      ELY_MUSTTAIL return lex_decimal(it, end, tok_start, out_start, out_end,
                                      out);
    } else if (is_delimiter(*it)) {
      out += encode<token_kind::integer_lit>(out, it - tok_start);
      DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      ELY_MUSTTAIL return lex_identifier(it, end, tok_start, out_start, out_end,
                                         out);
    }
  }
  ELY_MUSTTAIL return write_spill<cont::integer_lit>(it, end, tok_start,
                                                     out_start, out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_decimal(const char* it, const char* end, const char* tok_start,
            const std::uint8_t* out_start, const std::uint8_t* out_end,
            std::uint8_t* out) {
  for (; it != end; ++it) {
    if (is_delimiter(*it)) {
      out += encode<token_kind::decimal_lit>(out, it - tok_start);
      DISPATCH();
    } else if (!is_digit(*it)) {
      ++it;
      ELY_MUSTTAIL return lex_identifier(it, end, tok_start, out_start, out_end,
                                         out);
    }
  }
  ELY_MUSTTAIL return write_spill<cont::decimal_lit>(it, end, tok_start,
                                                     out_start, out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_string(const char* it, const char* end, const char* tok_start,
           const std::uint8_t* out_start, const std::uint8_t* out_end,
           std::uint8_t* out) {
  for (; it != end; ++it) {
    if (*it == '"') {
      ++it;
      out += encode<token_kind::string_lit>(out, it - tok_start);
      DISPATCH();
    }
  }
  ELY_MUSTTAIL return write_spill<cont::string_lit>(it, end, tok_start,
                                                    out_start, out_end, out);
}

constexpr std::size_t ELY_PRESERVE_NONE lex_string(const char*, const char*,
                                                   const char*,
                                                   const std::uint8_t*,
                                                   const std::uint8_t*,
                                                   std::uint8_t* out);

constexpr std::size_t ELY_PRESERVE_NONE
lex_newline_lf(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out) {
  out += encode<token_kind::newline_lf>(out);
  DISPATCH();
}

constexpr std::size_t ELY_PRESERVE_NONE
lex_newline_cr(const char* it, const char* end, const char* tok_start,
               const std::uint8_t* out_start, const std::uint8_t* out_end,
               std::uint8_t* out) {
  if (it == end) {
    ELY_MUSTTAIL return write_spill<cont::newline_cr>(it, end, tok_start,
                                                      out_start, out_end, out);
  }
  if (*it == '\n') {
    ++it;
    out += encode<token_kind::newline_crlf>(out);
  } else {
    out += encode<token_kind::newline_cr>(out);
  }

  DISPATCH();
}

template <char C>
constexpr std::size_t ELY_PRESERVE_NONE lex_paren(const char* it,
                                                  const char* end,
                                                  const char* tok_start,
                                                  const std::uint8_t* out_start,
                                                  const std::uint8_t* out_end,
                                                  std::uint8_t* out) {
  constexpr auto tok = [] {
    if constexpr (C == '(')
      return token_kind::lparen;
    else if constexpr (C == ')')
      return token_kind::rparen;
    else if constexpr (C == '[')
      return token_kind::lbracket;
    else if constexpr (C == ']')
      return token_kind::rbracket;
    else if constexpr (C == '{')
      return token_kind::lbrace;
    else if constexpr (C == '}')
      return token_kind::rbrace;
    else {
      static_assert(false, "unexpected character");
    }
  }();
  out += encode<tok>(out);
  DISPATCH();
}

constexpr std::size_t ELY_PRESERVE_NONE lex_start(const char* it,
                                                  const char* end,
                                                  const char* tok_start,
                                                  const std::uint8_t* out_start,
                                                  const std::uint8_t* out_end,
                                                  std::uint8_t* out) {
  DISPATCH();
}
} // namespace
} // namespace lexer2

constexpr std::size_t lex2(std::string_view src,
                           std::span<std::uint8_t> out_buffer,
                           std::uint8_t cont_id = 0) {
  if (out_buffer.size() < 4) {
    return 0;
  }

  const char* it = src.data();
  const char* end = src.data() + src.size();
  const char* tok_start = it;
  const std::uint8_t* out_start = out_buffer.data();
  const std::uint8_t* out_end = out_buffer.data() + out_buffer.size();
  std::uint8_t* out = out_buffer.data();

  return cont_table[cont_id](it, end, tok_start, out_start, out_end, out);
}
} // namespace stx
} // namespace ely