#pragma once

#include <cassert>
#include <cstdint>

#include "input.hpp"

namespace ely {
enum struct token_kind : std::uint16_t {
#define TOK(x, _) x,
#include "tokens.def"
};

constexpr const char* token_kind_name(token_kind k) {
  switch (k) {
#define TOK(x, _)                                                              \
  case token_kind::x:                                                          \
    return #x;
#include "tokens.def"
  }

  return nullptr;
}

constexpr const char* token_kind_short_name(token_kind k) {
  switch (k) {
#define TOK(x, name)                                                           \
  case token_kind::x:                                                          \
    return name;
#include "tokens.def"
  }

  return nullptr;
}

struct token {
  token_kind kind;
  std::uint16_t len;
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

template <typename Stream> class lexer {
public:
  using stream_type = Stream;

  using value_type = token;

private:
  stream_type stream_;
  using char_type = decltype(stream_.next());

public:
  explicit constexpr lexer(const stream_type& stream) : stream_(stream) {}
  explicit constexpr lexer(stream_type&& stream) : stream_(std::move(stream)) {}

  constexpr stream_type base() const { return stream_; }

  template <typename Buffer> constexpr value_type next(Buffer& buf) {
    std::size_t count = 0;

    auto make_token = [&](token_kind kind) -> token {
      return token{kind, static_cast<std::uint16_t>(count)};
    };

    auto c = next_char(count);

    switch (c) {
    case '\0':
      return make_token(token_kind::eof);
    case ' ':
      return read_whitespace(c);
    case '\t':
      return read_tab(c);
    case '\n':
      return make_token(token_kind::newline_lf);
    case '\r':
      c = peek_char();
      if (c != '\0') {
        if (c == '\n') {
          consume_char(count);
          return make_token(token_kind::newline_crlf);
        }
      }
      return make_token(token_kind::newline_cr);
    case '(':
      return make_token(token_kind::lparen);
    case ')':
      return make_token(token_kind::rparen);
    case '[':
      return make_token(token_kind::lbracket);
    case ']':
      return make_token(token_kind::rbracket);
    case '{':
      return make_token(token_kind::lbrace);
    case '}':
      return make_token(token_kind::rbrace);
    case '"':
      return read_string(c);
    default:
      if (detail::is_digit(c))
        return read_number(c);
      if (detail::is_identifier_start(c))
        return read_identifier(c);
      return make_token(token_kind::unknown);
    }
  }

private:
  constexpr char next_char(std::size_t& cnt) {
    ++cnt;
    return stream_.next();
  }

  constexpr char peek_char() { return stream_.peek(); }

  constexpr void consume_char(std::size_t& cnt) { next_char(cnt); }

  constexpr char consume_peek_char(std::size_t& cnt) {
    consume_char(cnt);
    return peek_char();
  }

  constexpr token read_whitespace(char_type c) {
    std::size_t cnt = 1;

    assert(c == ' ');
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (c != ' ')
        break;
    }

    return {token_kind::whitespace, static_cast<std::uint16_t>(cnt)};
  }

  constexpr token read_tab(char_type c) {
    std::size_t cnt = 1;

    assert(c == '\t');
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (c != '\t')
        break;
    }

    return {token_kind::tab, static_cast<std::uint16_t>(cnt)};
  }

  constexpr token read_identifier(char_type c) {
    std::size_t cnt = 1;
    assert(detail::is_identifier_start(c));
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (!detail::is_identifier_continue(c))
        break;
    }

    return {token_kind::identifier, static_cast<std::uint16_t>(cnt)};
  }

  constexpr token read_number(char_type c) {
    std::size_t cnt = 1;
    assert(detail::is_digit(c));
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (c == '.') {
        consume_char(cnt);
        return read_decimal(c, cnt);
      }
      if (!detail::is_digit(c))
        break;
    }

    return {token_kind::integer_lit, static_cast<std::uint16_t>(cnt)};
  }

  constexpr token read_decimal(char_type c, std::size_t cnt) {
    assert(c == '.');
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (!detail::is_digit(c))
        break;
    }

    return {token_kind::decimal_lit, static_cast<std::uint16_t>(cnt)};
  }

  constexpr token read_string(char_type c) {
    std::size_t cnt = 1;
    assert(c == '"');

    bool escaped = false;
    for (c = peek_char(); c != '\0'; c = consume_peek_char(cnt)) {
      if (c == '\\') {
        escaped = !escaped;
        continue;
      }

      if (c == '"' && !escaped) {
        consume_char(cnt);
        return {token_kind::string_lit, static_cast<std::uint16_t>(cnt)};
      }
      escaped = false;
    }
    return {token_kind::unterminated_string_lit,
            static_cast<std::uint16_t>(cnt)};
  }
};

lexer(std::string_view) -> lexer<ely::input_stream>;
lexer(const char*) -> lexer<ely::input_stream>;
} // namespace ely

// namespace test {
// using ely::token_kind;
// using token_kind::decimal_lit;
// using token_kind::eof;
// using token_kind::identifier;
// using token_kind::integer_lit;
// using token_kind::lbrace;
// using token_kind::lbracket;
// using token_kind::lparen;
// using token_kind::newline_cr;
// using token_kind::newline_crlf;
// using token_kind::newline_lf;
// using token_kind::rbrace;
// using token_kind::rbracket;
// using token_kind::rparen;
// using token_kind::string_lit;
// using token_kind::unterminated_string_lit;
// using token_kind::whitespace;

// constexpr bool ranges_equal(const char* src,
//                             std::initializer_list<ely::token_kind> ilist) {
//   auto lex = ely::lexer{src};
//   auto tok = lex.next();
//   auto it2 = ilist.begin();
//   auto end2 = ilist.end();

//   for (; tok.kind != eof && it2 != end2; tok = lex.next(), ++it2) {
//     if (tok.kind != *it2)
//       return false;
//   }

//   return tok.kind == eof && it2 == end2;
// }
// static_assert(ranges_equal(" ", {whitespace}));
// static_assert(ranges_equal("()", {lparen, rparen}));
// static_assert(ranges_equal("[ ] ",
//                            {lbracket, whitespace, rbracket, whitespace}));
// static_assert(ranges_equal("\n", {newline_lf}));
// static_assert(ranges_equal("\r", {newline_cr}));
// static_assert(ranges_equal("\r ", {newline_cr, whitespace}));
// static_assert(ranges_equal("\r\n", {newline_crlf}));
// static_assert(ranges_equal("hello", {identifier}));
// static_assert(ranges_equal("12343", {integer_lit}));
// static_assert(ranges_equal("123.4", {decimal_lit}));
// static_assert(ranges_equal("\"hello world\"", {string_lit}));
// static_assert(ranges_equal("\"escaped\\\"world\"", {string_lit}));
// static_assert(ranges_equal("\"random\\escape\"", {string_lit}));
// } // namespace test
