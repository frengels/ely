#pragma once

#include <cassert>
#include <optional>
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

struct scan_result {
  token_kind kind;
  std::string_view lexeme;
};

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

constexpr std::string_view make_strv(const char *begin, const char *end) {
  return std::string_view{begin,
                          static_cast<std::size_t>(std::distance(begin, end))};
}

struct whitespace_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ' '; };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());
    while (it != strv.end()) {
      auto ch = *it;

      if (ch != ' ') {
        break;
      }
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct tab_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '\t'; };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());
    for (; it != strv.end(); ++it) {
      auto ch = *it;

      if (ch != '\t') {
        break;
      }
    }

    return {.kind = token_kind::atmosphere,
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct line_comment_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ';'; };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());
    for (; it != strv.end(); ++it) {
      auto ch = *it;

      if (ch == '\r') {
        ++it;
        if (it != strv.end()) {
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
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct lparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '('; };

  static constexpr scan_result impl(std::string_view strv) {
    return {.kind = token_kind::lparen,
            .lexeme = make_strv(strv.begin(), std::next(strv.begin()))};
  }
};

struct rparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ')'; };

  static constexpr scan_result impl(std::string_view strv) {
    return {.kind = token_kind::rparen,
            .lexeme = make_strv(strv.begin(), std::next(strv.begin()))};
  }
};

struct identifier_lexer {
  static constexpr auto start_pred = [](auto ch) {
    return is_identifier_start(ch);
  };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());

    for (; it != strv.end(); ++it) {
      auto ch = *it;
      if (!is_identifier_continue(ch)) {
        break;
      }
    }

    return {.kind = token_kind::identifier,
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct integer_lexer {
  static constexpr auto start_pred = [](auto ch) { return is_num(ch); };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());

    for (; it != strv.end(); ++it) {
      auto ch = *it;
      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::integer_literal,
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct number_lexer {
  static constexpr auto start_pred = [](auto ch) { return is_num(ch); };

private:
  static constexpr scan_result continue_decimal(std::string_view strv,
                                                std::string_view::iterator it) {
    assert(*it == '.');
    ++it;

    for (; it != strv.end(); ++it) {
      auto ch = *it;

      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::decimal_literal,
            .lexeme = make_strv(strv.begin(), it)};
  }

public:
  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());

    for (; it != strv.end(); ++it) {
      auto ch = *it;

      if (ch == '.') {
        return continue_decimal(strv, it);
      }
      if (!is_num(ch)) {
        break;
      }
    }

    return {.kind = token_kind::integer_literal,
            .lexeme = make_strv(strv.begin(), it)};
  }
};

struct string_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '"'; };

  static constexpr scan_result impl(std::string_view strv) {
    auto it = std::next(strv.begin());

    bool escaping = false;

    for (; it != strv.end(); ++it) {
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
            .lexeme = make_strv(strv.begin(), it)};
  }
};

template <typename Lexer> struct safe_adapter {
  constexpr std::optional<scan_result> operator()(std::string_view strv) const {
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

constexpr scan_result lex(std::string_view src) {
  if (src.empty()) {
    return {.kind = token_kind::eof, .lexeme = {}};
  }
  auto ch = src.front();
  switch (ch) {
  case ' ':
    return detail::whitespace_lexer::impl(src);
  case '\t':
    return detail::tab_lexer::impl(src);
  case ';':
    return detail::line_comment_lexer::impl(src);
  case '"':
    return detail::string_lexer::impl(src);
  case '(':
    return detail::lparen_lexer::impl(src);
  case ')':
    return detail::rparen_lexer::impl(src);
  default:
    if (detail::is_identifier_start(ch)) {
      return detail::identifier_lexer::impl(src);
    } else if (detail::is_num(ch)) {
      return detail::number_lexer::impl(src);
    } else {
      return {.kind = token_kind::unknown_char,
              .lexeme = detail::make_strv(src.begin(), std::next(src.begin()))};
    }
  }
}
} // namespace wmc