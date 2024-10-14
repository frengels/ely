#pragma once

#include <cassert>

#include "ely/tokens.hpp"

namespace ely {
namespace detail {
template <typename CharT> constexpr bool is_digit(CharT c) {
  return '0' <= c && c <= '9';
}

template <typename CharT> constexpr bool is_lower_alpha(CharT c) {
  return 'a' <= c && c <= 'z';
}

template <typename CharT> constexpr bool is_upper_alpha(CharT c) {
  return 'A' <= c && c <= 'Z';
}

template <typename CharT> constexpr bool is_alpha(CharT c) {
  return is_lower_alpha(c) || is_upper_alpha(c);
}

template <typename CharT> constexpr bool is_identifier_start(CharT c) {
  switch (c) {
  case '_':
    return true;
  default:
    return is_alpha(c);
  }
}

template <typename CharT> constexpr bool is_identifier_continue(CharT c) {
  switch (c) {
  case '_':
  case '-':
    return true;
  default:
    return is_alpha(c) || is_digit(c);
  }
}
} // namespace detail

template <typename Stream> class lexer {
public:
  using stream_type = Stream;

  using value_type = ely::token;

private:
  stream_type stream_;

  using char_type = decltype(stream_.next());

public:
  explicit constexpr lexer(const stream_type& stream) : stream_(stream) {}
  explicit constexpr lexer(stream_type&& stream) : stream_(std::move(stream)) {}

  constexpr stream_type base() const { return stream_; }

  value_type next() {
    auto c = stream_.next();

    switch (c) {
    case '\0':
      return make_token<tokens::eof>();
    case ' ':
      return read_whitespace(c);
    case '\t':
      return read_tab(c);
    case '\n':
      return make_token<tokens::newline_lf>();
    case '\r':
      return read_cr(c);
    case '(':
      return make_token<tokens::lparen>();
    case ')':
      return make_token<tokens::rparen>();
    case '[':
      return make_token<tokens::lbracket>();
    case ']':
      return make_token<tokens::rbracket>();
    case '{':
      return make_token<tokens::lbrace>();
    case '}':
      return make_token<tokens::rbrace>();
    case '\'':
      return make_token<tokens::quote>();
    case '`':
      return make_token<tokens::quasiquote>();
    case ',':
      return read_unquote(c);
    case '#':
      return read_syntax(c);
    case '"':
      return read_string(c);
    default:
      if (detail::is_digit(c))
        return read_number(c);
      else if (detail::is_identifier_start(c))
        return read_identifier(c);
      else
        return make_token<tokens::unknown>(c);
    }
  }

private:
  template <typename T, typename... Args>
  constexpr value_type make_token(Args&&... args) {
    return value_type(std::in_place_type<T>, static_cast<Args&&>(args)...);
  }

  constexpr char_type next_char() { return stream_.next(); }

  constexpr char_type peek_char() { return stream_.peek(); }

  constexpr void consume_char() { next_char(); }

  constexpr char_type consume_peek_char() {
    consume_char();
    return peek_char();
  }

  constexpr tokens::whitespace read_whitespace(char_type c) {
    assert(c == ' ');
    std::size_t count = 1;
    for (c = peek_char(); c != '\0'; c = consume_peek_char(), ++count) {
      if (c != ' ')
        break;
    }

    return tokens::whitespace(count);
  }

  constexpr tokens::tab read_tab(char_type c) {
    assert(c == '\t');
    std::size_t count = 1;
    for (c = peek_char(); c != '\0'; c = consume_peek_char(), ++count) {
      if (c != '\t')
        break;
    }

    return tokens::tab(count);
  }

  constexpr token read_cr(char_type c) {
    assert(c == '\r');
    c = peek_char();
    if (c == '\n') {
      consume_char();
      return tokens::newline_crlf{};
    }
    return tokens::newline_cr{};
  }

  constexpr token read_unquote(char_type c) {
    assert(c == ',');
    c = peek_char();
    if (c == '@') {
      consume_char();
      return tokens::unquote_splicing{};
    }

    return tokens::unquote{};
  }

  constexpr token read_syntax(char_type c) {
    assert(c == '#');
    c = peek_char();
    switch (c) {
    case '\'':
      consume_char();
      return tokens::syntax{};
    case '`':
      consume_char();
      return tokens::quasisyntax{};
    case ',':
      consume_char();
      c = peek_char();
      if (c == '@') {
        consume_char();
        return tokens::unsyntax_splicing{};
      }
      return tokens::unsyntax{};
    default:
      return tokens::unknown{};
    }
  }

  constexpr tokens::identifier read_identifier(char_type c) {
    assert(detail::is_identifier_start(c));

    std::string text;
    text.push_back(c);

    for (c = peek_char(); c != '\0'; c = consume_peek_char()) {
      if (!detail::is_identifier_continue(c))
        break;
      text.push_back(c);
    }

    return tokens::identifier{std::move(text)};
  }

  constexpr token read_number(char_type c) {
    assert(detail::is_digit(c));

    std::string text;
    text.push_back(c);

    for (c = peek_char(); c != '\0'; c = consume_peek_char()) {
      if (c == '.') {
        consume_char();
        return read_decimal_cont(c, std::move(text));
      }
      if (!detail::is_digit(c))
        break;
      text.push_back(c);
    }

    return make_token<tokens::integer_lit>(std::move(text));
  }

  constexpr tokens::decimal_lit read_decimal_cont(char_type c,
                                                  std::string&& text) {
    assert(c == '.');

    text.push_back(c);

    for (c = peek_char(); c != '\0'; c = consume_peek_char()) {
      if (!detail::is_digit(c))
        break;
      text.push_back(c);
    }

    return tokens::decimal_lit(std::move(text));
  }

  constexpr token read_string(char_type c) {
    assert(c == '"');

    std::string text;

    bool escaped = false;

    for (c = peek_char(); c != '\0'; c = consume_peek_char()) {
      if (c == '\\') {
        escaped = !escaped;
        text.push_back(c);
        continue;
      }

      if (c == '"' && !escaped) {
        consume_char();
        return tokens::string_lit(std::move(text));
      }

      text.push_back(c);
      escaped = false;
    }

    return tokens::unterminated_string_lit(std::move(text));
  }
};
} // namespace ely