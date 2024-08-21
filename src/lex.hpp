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

template <std::input_iterator I, std::sentinel_for<I> S> class lexer_iterator {

public:
  using iterator_type = I;

  using iterator_category = std::forward_iterator_tag;

  using value_type = token_kind;

private:
  I it_;
  S end_;

public:
  constexpr lexer_iterator(I it, S end) : it_(it), end_(end) {}
  template <typename V>
  constexpr lexer_iterator(V v)
      : lexer_iterator(std::ranges::begin(v), std::ranges::end(v)) {}

  constexpr iterator_type base() const { return it_; }

  constexpr lexer_iterator& operator++() {
    // does nothing
    return *this;
  }

  constexpr lexer_iterator operator++(int) {
    auto ret = *this;
    ++(*this);
    return ret;
  }

  constexpr value_type operator*() {
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
    default:
      return token_kind::unknown_char;
    }
  }

  friend constexpr bool operator==(const lexer_iterator& self,
                                   const std::default_sentinel_t) {
    return self.it_ == self.end_;
  }

private:
  constexpr token_kind read_whitespace(std::iter_value_t<I> c) {
    assert(c == ' ');
    for (; it_ != end_; ++it_) {
      if (*it_ != ' ')
        break;
    }

    return token_kind::whitespace;
  }
};

template <typename V> class lexer {
  using iterator =
      lexer_iterator<std::ranges::iterator_t<V>, std::ranges::sentinel_t<V>>;

private:
  iterator it_;

public:
  constexpr lexer(V view) : it_(view) {}

  constexpr iterator begin() const { return it_; }

  constexpr std::default_sentinel_t end() const {
    return std::default_sentinel;
  }
};

template <typename Char, typename I, typename S>
constexpr token_kind read_identifier_continue(Char c, I& it, S end) {
  assert(is_identifier_continue(c));
  for (; it != end; ++it) {
    c = *it;
    if (!is_identifier_continue(c)) {
      break;
    }
  }

  return token_kind::identifier;
}

template <typename Char, typename I, typename S>
constexpr token_kind read_identifier_start(Char c, I& it, S end) {
  assert(is_identifier_start(c));
  if (it == end) {
    return token_kind::identifier;
  }
}

template <typename Char, typename I, typename S>
constexpr token_kind read_identifier(Char c, I it, S end) {
  return read_identifier_start(c, it, end);
}

// template <typename V> constexpr lex_result<V> lex_once(V text) {
//   auto it = text.begin();
//   auto end = text.end();

//   if (it == end) {
//     return {text, token_kind::eof};
//   }

//   auto c = *it++;
//   switch (c) {
//   case ' ':
//     return detail::read_whitespace(c, it, end);
//   case '\r':
//     if (it != end) {
//       c = *it;
//       if (c == '\n') {
//         ++it;
//         return {V{it, end}, token_kind::newline_crlf};
//       }
//     }
//   case '\n':
//     return {V{it, end}, token_kind::newline_lf};
//   case '(':
//     return {V{it, end}, token_kind::lparen};
//   case ')':
//     return {V{it, end}, token_kind::rparen};
//   case '[':
//     return {V{it, end}, token_kind::lbracket};
//   case ']':
//     return {V{it, end}, token_kind::rbracket};
//   case '{':
//     return {V{it, end}, token_kind::lbrace};
//   case '}':
//     return {V{it, end}, token_kind::rbrace};
//   default:
//     return token_kind::unknown_char;
//   }
// }
} // namespace ely

namespace test {
using ely::token_kind;
using token_kind::lbrace;
using token_kind::lbracket;
using token_kind::lparen;
using token_kind::newline_cr;
using token_kind::newline_crlf;
using token_kind::newline_lf;
using token_kind::rbrace;
using token_kind::rbracket;
using token_kind::rparen;
using token_kind::whitespace;

constexpr bool ranges_equal(std::string_view src,
                            std::initializer_list<ely::token_kind> ilist) {
  auto lex = ely::lexer{src};
  auto it1 = lex.begin();
  auto end1 = lex.end();
  auto it2 = ilist.begin();
  auto end2 = ilist.end();

  for (; it1 != end1 && it2 != end2; ++it1, ++it2) {
    if (*it1 != *it2)
      return false;
  }

  return it1 == end1 && it2 == end2;
}
static_assert(ranges_equal(" ", {whitespace}));
static_assert(ranges_equal("()", {lparen, rparen}));
static_assert(ranges_equal("[ ] ",
                           {lbracket, whitespace, rbracket, whitespace}));
static_assert(ranges_equal("\n", {newline_lf}));
static_assert(ranges_equal("\r", {newline_cr}));
static_assert(ranges_equal("\r ", {newline_cr, whitespace}));
static_assert(ranges_equal("\r\n", {newline_crlf}));
} // namespace test
