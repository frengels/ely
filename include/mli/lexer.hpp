#pragma once

#include <cassert>
#include <optional>
#include <ranges>
#include <string_view>

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

template <typename V> struct basic_token {
  token_kind kind;
  V lexeme;
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

    return {.token = {.kind = token_kind::atmosphere,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::atmosphere,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::atmosphere,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::atmosphere,
                      .lexeme = detail::make_view<V>(src.begin(), it)},
            .next = detail::make_view<V>(it, src.end())};
  }
};

struct lparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == '('; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    return {
        .token = {.kind = token_kind::lparen,
                  .lexeme = make_view<V>(src.begin(), std::next(src.begin()))},
        .next = make_view<V>(std::next(src.begin()), src.end())};
  }
};

struct rparen_lexer {
  static constexpr auto start_pred = [](auto ch) { return ch == ')'; };

  template <typename V> static constexpr basic_scan_result<V> impl(V src) {
    return {
        .token = {.kind = token_kind::rparen,
                  .lexeme = make_view<V>(src.begin(), std::next(src.begin()))},
        .next = make_view<V>(std::next(src.begin()), src.end())};
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

    return {.token = {.kind = token_kind::identifier,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::integer_literal,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::decimal_literal,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::integer_literal,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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

    return {.token = {.kind = token_kind::string_literal,
                      .lexeme = make_view<V>(src.begin(), it)},
            .next = make_view<V>(it, src.end())};
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
    return {.token = {.kind = token_kind::eof, .lexeme = {}}, .next = src};
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
    return {.token = {.kind = token_kind::eof, .lexeme = {}}, .next = src};
  default:
    if (detail::identifier_lexer::start_pred(ch)) {
      return detail::identifier_lexer::impl(src);
    } else if (detail::number_lexer::start_pred(ch)) {
      return detail::number_lexer::impl(src);
    } else {
      return {.token = {.kind = token_kind::unknown_char,
                        .lexeme = detail::make_view<V>(it, std::next(it))},
              .next = detail::make_view<V>(std::next(it), src.end())};
    }
  }
}

struct source_position {
  // both line and col should start at 1, line 0 means no position was given
  uint32_t line{};
  uint32_t col{};

  source_position() = default;

  constexpr source_position(uint32_t line, uint32_t col)
      : line(line), col(col) {}

  friend bool operator==(const source_position &,
                         const source_position &) = default;
};

template <typename V> class basic_source_view {
  V src_;
  source_position pos_;

public:
  class iterator {
    using it_traits = std::iterator_traits<std::ranges::iterator_t<V>>;

  public:
    using value_type = typename it_traits::value_type;
    using reference = typename it_traits::reference;
    using pointer = typename it_traits::pointer;
    using difference_type = typename it_traits::difference_type;
    using iterator_category = std::forward_iterator_tag;

  private:
    std::ranges::iterator_t<V> it_;
    source_position pos_;

  public:
    iterator() = default;

    constexpr iterator(std::ranges::iterator_t<V> it, source_position pos)
        : it_(it), pos_(pos) {}

    constexpr const source_position &pos() const { return pos_; }
    constexpr const std::ranges::iterator_t<V> &base() const { return it_; }

    friend constexpr bool operator==(const iterator &lhs, const iterator &rhs) {
      return lhs.it_ == rhs.it_;
    }

    constexpr iterator &operator++() {
      auto ch = *it_;
      switch (ch) {
      case '\n':
        ++pos_.line;
        pos_.col = 1;
        break;
      default:
        ++pos_.col;
        break;
      }

      ++it_;
      return *this;
    }

    constexpr iterator operator++(int) {
      auto tmp = *this;
      ++*this;
      return tmp;
    }

    constexpr std::ranges::range_reference_t<V> operator*() const {
      return *it_;
    }
  };

public:
  basic_source_view() = default;
  constexpr basic_source_view(V src) : src_(src), pos_(source_position{1, 1}) {}
  constexpr basic_source_view(iterator first, iterator end)
      : src_(detail::make_view<V>(first.base(), end.base())),
        pos_(first.pos()) {}

  constexpr iterator begin() const { return iterator{src_.begin(), pos()}; }
  constexpr iterator end() const {
    return iterator{src_.end(), source_position{}};
  }

  constexpr const source_position &pos() const { return pos_; }
};

using source_view = basic_source_view<std::string_view>;

template <typename V> class basic_lexer {
private:
  V src_;

public:
  basic_lexer() = default;
  explicit constexpr basic_lexer(V src) : src_(std::move(src)) {}

  constexpr const V &src() const { return src_; }

  constexpr basic_token<V> next() {
    basic_scan_result<V> res;

    do {
      res = lex(src_);
      src_ = res.next;
    } while (res.token.kind == token_kind::atmosphere);

    return res.token;
  }
};

using lexer = basic_lexer<std::string_view>;
using pos_lexer = basic_lexer<source_view>;
} // namespace mli