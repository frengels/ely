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
};

struct scan_result {
  token_kind kind;
  std::string_view lexeme;
};

namespace detail {
constexpr bool is_num(auto ch) { return '0' <= ch && ch <= '9'; }

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

constexpr auto lex_string = detail::safe_adapter<detail::string_lexer>{};

constexpr scan_result lex(std::string_view src) {
  for (auto ch : src) {
    switch (ch) {
    case ' ':
      return detail::whitespace_lexer::impl(src);
    }
  }
}
} // namespace wmc