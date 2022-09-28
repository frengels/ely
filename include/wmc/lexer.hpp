#pragma once

#include <cassert>
#include <optional>
#include <ranges>
#include <string_view>

namespace wmc {
enum class token_kind {
  atmosphere,

  identifier,
  integer_literal,
  decimal_literal,
  string_literal,

  lparen,
  rparen,

  eof,

  unknown_char,
};

template <typename V> struct basic_scan_result {
  token_kind kind;
  V lexeme;
};

using scan_result = basic_scan_result<std::string_view>;

namespace detail {
constexpr bool is_num(auto ch) { return '0' <= ch && ch <= '9'; }

constexpr bool is_lower_alpha(auto ch) { return 'a' <= ch && ch <= 'z'; }

constexpr bool is_upper_alpha(auto ch) { return 'A' <= ch && ch <= 'Z'; }

constexpr bool is_alpha(auto ch) {
  return is_lower_alpha(ch) || is_upper_alpha(ch);
}

constexpr bool is_special_initial(auto ch) {
  switch (ch) {
  case '*':
  case '+':
  case '-':
  case '_':
  case '/':
    return true;
  default:
    return false;
  }
}

constexpr bool is_identifier_start(auto ch) {
  return is_alpha(ch) || is_special_initial(ch);
}

constexpr bool is_identifier_continue(auto ch) {
  return is_identifier_start(ch) || is_num(ch);
}

constexpr std::string_view make_view(std::string_view::iterator begin,
                                     std::string_view::iterator end) {
  return std::string_view{std::to_address(begin),
                          static_cast<std::size_t>(std::distance(begin, end))};
}

template <typename V, typename I, typename S>
constexpr V make_view(I begin, S end) {
  return V{begin, end};
}

struct whitespace_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ' '; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());
    while (it != src.end()) {
      auto ch = *it;

      if (ch != ' ') {
        break;
      }
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct tab_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '\t'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());
    for (; it != src.end(); ++it) {
      auto ch = *it;

      if (ch != '\t') {
        break;
      }
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct line_comment_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ';'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());
    for (; it != src.end(); ++it) {
      auto ch = *it;

      if (ch == '\r') {
        ++it;
        if (it != src.end()) {
          ch = *it;
          if (ch == '\n') {
            ++it;
          }
        }

        break;
      } else if (ch == '\n') {
        ++it;
        break;
      }
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct newline_lexer {
  static constexpr auto start_pred = [](auto ch) {
    return ch == '\r' || ch == '\n';
  };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = src.begin();
    switch (*it) {
    case '\r':
      ++it;
      if (*it == '\n') {
        ++it;
      }
      break;
    case '\n':
      ++it;
      break;
    default:
      __builtin_unreachable();
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = detail::make_view<V>(src.begin(), it)};
  }
};

struct lparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '('; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    return {.kind = token_kind::lparen,
            .lexeme = make_view<V>(src.begin(), std::next(src.begin()))};
  }
};

struct rparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ')'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    return {.kind = token_kind::rparen,
            .lexeme = make_view<V>(src.begin(), std::next(src.begin()))};
  }
};

struct identifier_lexer {
  static constexpr auto start_pred = [](auto ch) {
    return is_identifier_start(ch);
  };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());

    for (; it != src.end(); ++it) {
      auto ch = *it;
      if (!is_identifier_continue(ch)) {
        break;
      }
    }

    return {.kind = token_kind::identifier,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct integer_lexer {
  static constexpr auto start_pred = [](auto ch) { return is_num(ch); };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());

    for (; it != src.end(); ++it) {
      auto ch = *it;
      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::integer_literal,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct number_lexer {
  static constexpr auto start_pred = [](auto ch) { return is_num(ch); };

private:
  template <typename V>
  static constexpr basic_scan_result<V>
  continue_decimal(V src, std::ranges::iterator_t<V> it) {
    assert(*it == '.');
    ++it;

    for (; it != src.end(); ++it) {
      auto ch = *it;

      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::decimal_literal,
            .lexeme = make_view<V>(src.begin(), it)};
  }

public:
  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());

    for (; it != src.end(); ++it) {
      auto ch = *it;

      if (ch == '.') {
        return continue_decimal(src, it);
      }
      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::integer_literal,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

struct string_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '"'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto it = std::next(src.begin());
    bool escaping = false;

    for (; it != src.end(); ++it) {
      auto ch = *it;
      if (ch == '\\') {
        escaping = !escaping;
        continue;
      } else if (ch == '"' && !escaping) {
        ++it;
        break;
      }

      escaping = false;
    }

    return {.kind = token_kind::string_literal,
            .lexeme = make_view<V>(src.begin(), it)};
  }
};

template <typename Lexer> struct safe_adapter {
  template <typename V>
  constexpr std::optional<basic_scan_result<V>> operator()(V strv) const {
    if (strv.empty() || !Lexer::start_pred(strv.front())) {
      return std::nullopt;
    }

    return Lexer::impl(strv);
  }
};
} // namespace detail

constexpr auto lex_whitespace =
    detail::safe_adapter<detail::whitespace_lexer>{};
constexpr auto lex_tabs = detail::safe_adapter<detail::tab_lexer>{};
constexpr auto lex_line_comment =
    detail::safe_adapter<detail::line_comment_lexer>{};
constexpr auto lex_identifier =
    detail::safe_adapter<detail::identifier_lexer>{};
constexpr auto lex_number = detail::safe_adapter<detail::number_lexer>{};
constexpr auto lex_string = detail::safe_adapter<detail::string_lexer>{};

template <typename V> constexpr basic_scan_result<V> lex(V src) {
  auto it = src.begin();
  auto end = src.end();
  if (it == end) {
    return {.kind = token_kind::eof, .lexeme = {}};
  }

  auto ch = *it;
  switch (ch) {
  case ' ':
    return detail::whitespace_lexer::impl(src);
  case '\t':
    return detail::tab_lexer::impl(src);
  case '\r':
  case '\n':
    return detail::newline_lexer::impl(src);
  case ';':
    return detail::line_comment_lexer::impl(src);
  case '"':
    return detail::string_lexer::impl(src);
  case '(':
    return detail::lparen_lexer::impl(src);
  case ')':
    return detail::rparen_lexer::impl(src);
  case '\0':
    return {.kind = token_kind::eof, .lexeme = {}};
  default:
    if (detail::identifier_lexer::start_pred(ch)) {
      return detail::identifier_lexer::impl(src);
    } else if (detail::number_lexer::start_pred(ch)) {
      return detail::number_lexer::impl(src);
    } else {
      return {.kind = token_kind::unknown_char,
              .lexeme = detail::make_view<V>(it, std::next(it))};
    }
  }
}
} // namespace wmc