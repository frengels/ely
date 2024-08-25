#pragma once

#include <cassert>
#include <concepts>
#include <iterator>
#include <ranges>

namespace ely {
enum struct token_kind {
  whitespace,
  newline_lf,
  newline_cr,
  newline_crlf,

  lparen,
  rparen,
  lbracket,
  rbracket,
  lbrace,
  rbrace,

  identifier,

  integer_lit,
  decimal_lit,
  string_lit,

  unterminated_string_lit,
  unknown_char,

  eof,
};

namespace detail {
template <typename Char> constexpr bool is_lower_alpha(Char c) {
  return 'a' <= c && c <= 'z';
}

template <typename Char> constexpr bool is_upper_alpha(Char c) {
  return 'A' <= c && c <= 'Z';
}

template <typename Char> constexpr bool is_alpha(Char c) {
  return is_lower_alpha(c) || is_upper_alpha(c);
}

template <typename Char> constexpr bool is_digit(Char c) {
  return '0' <= c && c <= '9';
}

template <typename Char> constexpr bool is_identifier_start(Char c) {
  switch (c) {
  case '_':
    return true;
  default:
    return is_alpha(c);
  }
}

template <typename Char> constexpr bool is_identifier_continue(Char c) {
  switch (c) {
  case '_':
  case '-':
    return true;
  default:
    return is_alpha(c) || is_digit(c);
  }
}
} // namespace detail

template <typename V> class lexer {

public:
  using view_type = V;

  using value_type = token_kind;

private:
  [[no_unique_address]] std::ranges::iterator_t<V> it_;
  [[no_unique_address]] std::ranges::sentinel_t<V> end_;

public:
  constexpr lexer(std::ranges::iterator_t<V> it, std::ranges::sentinel_t<V> end)
      : it_(it), end_(end) {}
  explicit constexpr lexer(V v)
      : lexer(std::ranges::begin(v), std::ranges::end(v)) {}

  constexpr view_type base() const { return view_type{it_, end_}; }

  constexpr value_type next() {
    if (it_ == end_)
      return token_kind::eof;

    auto c = *it_++;
    switch (c) {
    case ' ':
      return read_whitespace(c);
    case '\n':
      return token_kind::newline_lf;
    case '\r':
      if (it_ != end_) {
        c = *it_;
        if (c == '\n') {
          ++it_;
          return token_kind::newline_crlf;
        }
      }
      return token_kind::newline_cr;
    case '(':
      return token_kind::lparen;
    case ')':
      return token_kind::rparen;
    case '[':
      return token_kind::lbracket;
    case ']':
      return token_kind::rbracket;
    case '{':
      return token_kind::lbrace;
    case '}':
      return token_kind::rbrace;
    case '"':
      return read_string(c);
    default:
      if (detail::is_digit(c))
        return read_number(c);
      if (detail::is_identifier_start(c))
        return read_identifier(c);
      return token_kind::unknown_char;
    }
  }

private:
  constexpr token_kind read_whitespace(std::ranges::range_value_t<V> c) {
    assert(c == ' ');
    for (; it_ != end_; ++it_) {
      if (*it_ != ' ')
        break;
    }

    return token_kind::whitespace;
  }

  constexpr token_kind read_identifier(std::ranges::range_value_t<V> c) {
    assert(detail::is_identifier_start(c));
    for (; it_ != end_; ++it_) {
      if (!detail::is_identifier_continue(c))
        break;
    }

    return token_kind::identifier;
  }

  constexpr token_kind read_number(std::ranges::range_value_t<V> c) {
    assert(detail::is_digit(c));
    for (; it_ != end_; ++it_) {
      c = *it_;
      if (c == '.') {
        ++it_;
        return read_decimal(c);
      }
      if (!detail::is_digit(c))
        break;
    }

    return token_kind::integer_lit;
  }

  constexpr token_kind read_decimal(std::ranges::range_value_t<V> c) {
    assert(c == '.');
    for (; it_ != end_; ++it_) {
      c = *it_;
      if (!detail::is_digit(c))
        break;
    }

    return token_kind::decimal_lit;
  }

  constexpr token_kind read_string(std::ranges::range_value_t<V> c) {
    assert(c == '"');

    bool escaped = false;
    for (; it_ != end_; ++it_) {
      c = *it_;
      if (c == '\\') {
        escaped = !escaped;
        continue;
      }

      if (c == '"' && !escaped) {
        ++it_;
        return token_kind::string_lit;
      }
      escaped = false;
    }
    return token_kind::unterminated_string_lit;
  }
};
} // namespace ely

namespace test {
using ely::token_kind;
using token_kind::decimal_lit;
using token_kind::eof;
using token_kind::identifier;
using token_kind::integer_lit;
using token_kind::lbrace;
using token_kind::lbracket;
using token_kind::lparen;
using token_kind::newline_cr;
using token_kind::newline_crlf;
using token_kind::newline_lf;
using token_kind::rbrace;
using token_kind::rbracket;
using token_kind::rparen;
using token_kind::string_lit;
using token_kind::unterminated_string_lit;
using token_kind::whitespace;

constexpr bool ranges_equal(std::string_view src,
                            std::initializer_list<ely::token_kind> ilist) {
  auto lex = ely::lexer{src};
  auto tok = lex.next();
  auto it2 = ilist.begin();
  auto end2 = ilist.end();

  for (; tok != eof && it2 != end2; tok = lex.next(), ++it2) {
    if (tok != *it2)
      return false;
  }

  return tok == eof && it2 == end2;
}
static_assert(ranges_equal(" ", {whitespace}));
static_assert(ranges_equal("()", {lparen, rparen}));
static_assert(ranges_equal("[ ] ",
                           {lbracket, whitespace, rbracket, whitespace}));
static_assert(ranges_equal("\n", {newline_lf}));
static_assert(ranges_equal("\r", {newline_cr}));
static_assert(ranges_equal("\r ", {newline_cr, whitespace}));
static_assert(ranges_equal("\r\n", {newline_crlf}));
static_assert(ranges_equal("hello", {identifier}));
static_assert(ranges_equal("12343", {integer_lit}));
static_assert(ranges_equal("123.4", {decimal_lit}));
static_assert(ranges_equal("\"hello world\"", {string_lit}));
static_assert(ranges_equal("\"escaped\\\"world\"", {string_lit}));
static_assert(ranges_equal("\"random\\escape\"", {string_lit}));
} // namespace test
