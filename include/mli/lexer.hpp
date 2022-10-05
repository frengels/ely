#pragma once

#include <cassert>
#include <compare>
#include <optional>
#include <ranges>

#include "mli/string_view.hpp"

namespace mli {
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

struct source_position {
  // both line and col should start at 1, line 0 means no position was given
  uint32_t line{};
  uint32_t col{};

  source_position() = default;

  constexpr source_position(uint32_t line, uint32_t col)
      : line(line), col(col) {}

  friend bool operator==(const source_position &,
                         const source_position &) = default;

  explicit constexpr operator bool() const noexcept { return line != 0; }

  template <typename Out>
  friend constexpr Out &operator<<(Out &out, const source_position &pos) {
    out << "(position ";
    if (pos.line == 0) {
      return out << "?:?)";
    } else {
      out << pos.line << ":";
    }

    if (pos.col == 0) {
      return out << "?)";
    } else {
      return out << pos.col << ")";
    }
  }
};

struct source_offset {
  using value_type = std::uint32_t;

  static constexpr value_type npos = std::numeric_limits<value_type>::max();

  value_type offset_{npos};

  source_offset() = default;

  constexpr source_offset(value_type offs) : offset_(offs) {}

  constexpr value_type value() const { return offset_; }
  explicit constexpr operator bool() const noexcept { return offset_ != npos; }
  constexpr operator std::uint32_t() const noexcept { return offset_; }

  friend bool operator==(const source_offset &,
                         const source_offset &) = default;
  friend constexpr std::partial_ordering
  operator<=>(const source_offset &lhs, const source_offset &rhs) noexcept {
    if (!lhs || !rhs) {
      return std::partial_ordering::unordered;
    }

    return lhs.value() <=> rhs.value();
  }

  template <typename Out>
  friend constexpr Out &operator<<(Out &out, const source_offset &offs) {
    out << "(offset ";
    if (!offs) {
      return out << "?)";
    } else {
      return out << offs.value() << ")";
    }
  }
};

template <typename V> class basic_token {
public:
  using iterator = std::ranges::iterator_t<V>;
  using size_type = std::make_unsigned_t<std::ranges::range_difference_t<V>>;

private:
  token_kind kind_{token_kind::eof};
  std::uint32_t size_{};
  std::ranges::iterator_t<V> it_{};

public:
  basic_token() = default;
  constexpr basic_token(token_kind kind, std::ranges::iterator_t<V> it,
                        std::uint32_t size = 0)
      : kind_(kind), size_(size), it_(it) {}

  constexpr basic_token(token_kind kind, std::ranges::iterator_t<V> it,
                        std::ranges::range_difference_t<V> size = 0)
      : basic_token(kind, it, static_cast<std::uint32_t>(size)) {}

  constexpr token_kind kind() const { return kind_; }
  constexpr size_type size() const { return static_cast<size_type>(size_); }
  constexpr iterator begin() const { return it_; }
  constexpr iterator end() const { return it_ + size(); }

  explicit constexpr operator bool() const noexcept {
    return kind() != token_kind::eof;
  }
};

template <typename V> struct basic_scan_result {
  basic_token<V> token;
  V next;
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

struct whitespace_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ' '; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto start = src.begin();
    assert(start_pred(*start));
    auto it = std::next(start);
    std::uint32_t dist = 1;
    for (; it != src.end(); ++it, ++dist) {
      auto ch = *it;

      if (ch != ' ') {
        break;
      }
    }

    return {.token = {token_kind::atmosphere, start, dist},
            .next = {it, src.end()}};
  }
};

struct tab_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '\t'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    auto start = src.begin();
    assert(start_pred(*start));
    auto it = std::next(start);
    std::uint32_t dist = 1;
    for (; it != src.end(); ++it, ++dist) {
      auto ch = *it;

      if (ch != '\t') {
        break;
      }
    }

    return {.token = {token_kind::atmosphere, start, dist},
            .next = {it, src.end()}};
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

    return {.token = {token_kind::atmosphere, src.begin(), it - src.begin()},
            .next = {it, src.end()}};
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

    return {.token = {token_kind::atmosphere, src.begin(), it - src.begin()},
            .next = {it, src.end()}};
  }
};

struct lparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '('; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    assert(start_pred(*src.begin()));
    return {.token = {token_kind::lparen, src.begin(),
                      std::ranges::range_difference_t<V>{1}},
            .next = {std::next(src.begin()), src.end()}};
  }
};

struct rparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ')'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    assert(start_pred(*src.begin()));
    return {.token = {token_kind::rparen, src.begin(),
                      std::ranges::range_difference_t<V>{1}},
            .next = {std::next(src.begin()), src.end()}};
  }
};

struct identifier_lexer {
  static constexpr auto start_pred = [](auto ch) {
    return is_identifier_start(ch);
  };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    assert(start_pred(*src.begin()));
    auto it = std::next(src.begin());

    for (; it != src.end(); ++it) {
      auto ch = *it;
      if (!is_identifier_continue(ch)) {
        break;
      }
    }

    return {.token = {token_kind::identifier, src.begin(), it - src.begin()},
            .next = {it, src.end()}};
  }
};

struct integer_lexer {
  static constexpr auto start_pred = [](auto ch) { return is_num(ch); };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    assert(start_pred(*src.begin()));
    auto it = std::next(src.begin());

    for (; it != src.end(); ++it) {
      auto ch = *it;
      if (!is_num(ch)) {
        break;
      }
    }

    return {
        .token = {token_kind::integer_literal, src.begin(), it - src.begin()},
        .next = {it, src.end()}};
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

    return {
        .token = {token_kind::decimal_literal, src.begin(), it - src.begin()},
        .next = {it, src.end()}};
  }

public:
  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    assert(start_pred(*src.begin()));
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

    return {
        .token = {token_kind::integer_literal, src.begin(), it - src.begin()},
        .next = {it, src.end()}};
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

    return {
        .token = {token_kind::string_literal, src.begin(), it - src.begin()},
        .next = {it, src.end()}};
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
    return {
        .token = {token_kind::eof, it, std::ranges::range_difference_t<V>{}},
        .next = src};
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
    return {
        .token = {token_kind::eof, it, std::ranges::range_difference_t<V>{}},
        .next = src};
  default:
    if (detail::identifier_lexer::start_pred(ch)) {
      return detail::identifier_lexer::impl(src);
    } else if (detail::number_lexer::start_pred(ch)) {
      return detail::number_lexer::impl(src);
    } else {
      return {.token = {token_kind::unknown_char, it,
                        std::ranges::range_difference_t<V>{1}},
              .next = {std::next(it), src.end()}};
    }
  }
}

template <typename V> class basic_lex_result {
  basic_token<V> token_;
  std::uint32_t preceding_atmosphere_; // atmosphere that was ignored

public:
  constexpr basic_lex_result(basic_token<V> token,
                             std::size_t preceding_atmosphere)
      : token_(token), preceding_atmosphere_(
                           static_cast<std::uint32_t>(preceding_atmosphere)) {}

  constexpr token_kind kind() const { return token_.kind(); }
  constexpr auto size() const { return token_.size(); }
  constexpr basic_token<V> token() const { return token_; }
  constexpr auto begin() const { return token_.begin(); }
  constexpr auto end() const { return token_.end(); }
  constexpr std::size_t preceding_atmosphere() const {
    return static_cast<std::size_t>(preceding_atmosphere);
  }
};

template <typename V> class basic_lexer {
private:
  V src_;
  source_offset offset_;

public:
  basic_lexer() = default;
  explicit constexpr basic_lexer(V src) : src_(std::move(src)) {}

  constexpr const V &src() const { return src_; }
  constexpr const source_offset &offset() const { return offset_; }

  constexpr basic_lex_result<V> next() {
    basic_scan_result<V> res;
    std::uint32_t skipped = 0;

    do {
      res = lex(src_);
      src_ = res.next;
    } while (res.token.kind() == token_kind::atmosphere);

    return {res.token, skipped};
  }
};

using lexer = basic_lexer<mli::string_view>;
} // namespace mli