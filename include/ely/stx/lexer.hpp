#pragma once

#include <cassert>
#include <functional>
#include <optional>
#include <string_view>
#include <utility>

#include <fmt/core.h>

#include "ely/stx/tokens.hpp"

#include "ely/util/check.hpp"
#include "ely/util/optional.hpp"

namespace ely {
namespace stx {
namespace detail {
template <typename CharT> constexpr bool is_digit(CharT c) {
  return '0' <= c && c <= '9';
}

template <typename CharT> constexpr bool is_delimiter(CharT c) {
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
} // namespace detail

using token = ely::variant<
#define TOKEN(x) ely::stx::tokens::x,
#include "tokens.def"
#undef TOKEN
    >;

enum struct token_kind {
#define TOKEN(x) x,
#include "tokens.def"
};

constexpr const char* token_kind_name(token_kind k) {
  switch (k) {
#define TOKEN(x)                                                               \
  case token_kind::x:                                                          \
    return #x;
#include "tokens.def"
  default:
    return "<unknown>";
  }
}

constexpr bool token_is_newline(token_kind k) {
  switch (k) {
  case token_kind::newline_lf:
  case token_kind::newline_cr:
  case token_kind::newline_crlf:
    return true;
  default:
    return false;
  }
}

constexpr bool token_is_atmosphere(token_kind k) {
  switch (k) {
  case token_kind::whitespace:
  case token_kind::tab:
    return true;
  default:
    return token_is_newline(k);
  }
}

constexpr bool token_is_eof(token_kind tk) { return tk == token_kind::eof; }
constexpr bool token_is_identifier(token_kind tk) {
  return tk == token_kind::identifier;
}

struct lexer_result {
  token_kind token;
  char next;
};

class lexer {
public:
  using value_type = token_kind;

private:
  std::string_view::iterator it_;
  std::string_view::iterator end_;
  char lookahead_;

public:
  lexer() = default;

  explicit constexpr lexer(std::string_view block)
      : it_(block.begin()), end_(block.end()) {}

  constexpr auto base() const { return std::string_view{it_, end_}; }

  friend bool operator==(const lexer&, const lexer&) = default;
  friend auto operator<=>(const lexer&, const lexer&) = default;

  constexpr value_type next() {
    return next_byte_cont([&](char c) -> value_type {
      switch (c) {
      case '\0':
        // reached the actual end
        return token_kind::eof;
      case ' ':
        return next_whitespace();
      case '\t':
        return lex_tab();
      case '\n':
        consume_byte();
        return token_kind::newline_lf;
      case '\r':
        return lex_cr();
      case '(':
        consume_byte();
        return token_kind::lparen;
      case ')':
        consume_byte();
        return token_kind::rparen;
      case '[':
        consume_byte();
        return token_kind::lbracket;
      case ']':
        consume_byte();
        return token_kind::rbracket;
      case '{':
        consume_byte();
        return token_kind::lbrace;
      case '}':
        consume_byte();
        return token_kind::rbrace;
      case '\'':
        consume_byte();
        return token_kind::quote;
      case '`':
        consume_byte();
        return token_kind::quasiquote;
      case '$':
        consume_byte();
        return token_kind::dollar;
      case '/':
        consume_byte();
        return token_kind::slash;
      case ',':
        return lex_unquote();
      case '#':
        return lex_syntax();
      case '"':
        return lex_string();
      default:
        if (detail::is_digit(c))
          return lex_number_or_identifier();
        else
          return lex_identifier();
      }
    });
  }

private:
  constexpr ely::optional<char> peek_byte() {
    if (it_ == end_) {
      return std::nullopt;
    }
    return *it_;
  }

  template <typename F> constexpr token_kind peek_byte_cont(F&& fn) {
    auto opt_c = peek_byte();
    if (opt_c) {
      return std::invoke(static_cast<F&&>(fn), *opt_c);
    } else {
      return token_kind::need_input;
    }
  }

  constexpr void consume_byte() { ++it_; }

  template <typename F> constexpr token_kind next_byte_cont(F&& fn) {
    auto opt_c = peek_byte();
    if (opt_c) {
      consume_byte();
      std::invoke(static_cast<F&&>(fn), *opt_c);
    } else {
      return token_kind::need_input;
    }
  }

  // advance until `pred(char)` evaluates as true. Then returns nullopt, can
  // also return just<need_input> if the input stream runs out of characters.
  template <typename T, typename P>
  constexpr ely::optional<T> advance_until(P pred) {
    // get the byte, if there is none it will go to value_or and return
    // need_input, otherwise go into and_then where the predicate is checked. If
    // pred(c) == true return T as we're done advancing, otherwise consume
    // the current byte and enter recursion for the next byte.
    return peek_byte().and_then([&](char c) {
      if (pred(c)) {
        return T{};
      }

      consume_byte();
      return advance_until<T>(std::move(pred));
    });
  }

  template <typename CheckT>
  constexpr ely::optional<
      ely::variant<ely::tokens::whitespace, ely::tokens::need_input>>
  next_whitespace(CheckT = ely::check) {
    peek_byte()
        .and_then([&](char c) {
      maybe_check<CheckT>([&] -> bool { return c == ' '; })
          .transform([&] -> ely::variant<ely::tokens::whitespace,
                                         ely::tokens::need_input> {
            consume_byte();
            return advance_until<ely::tokens::whitespace>(
                       [](char c) { return c != ' '; })
                .value_or(ely::tokens::need_input{});
          });
      }
  }

  // constexpr value_type lex_tab() {
  //   assert(*start == '\t');
  //   for (; *it_ == '\t'; ++it_) {
  //   }
  //   return make_lexeme(token_kind::tab, start);
  // }

  // constexpr value_type lex_cr() {
  //   assert(*start == '\r');
  //   if (*it_ == '\n') {
  //     ++it_;
  //     return make_lexeme(token_kind::newline_crlf, start);
  //   }
  //   return make_lexeme(token_kind::newline_cr, start);
  // }

  // constexpr value_type lex_unquote() {
  //   assert(*start == ',');
  //   if (*it_ == '@') {
  //     ++it_;
  //     return make_lexeme(token_kind::unquote_splicing, start);
  //   }

  //   return make_lexeme(token_kind::unquote, start);
  // }

  // constexpr value_type lex_syntax(const char* start) {
  //   assert(*start == '#');
  //   switch (*it_) {
  //   case '\'':
  //     ++it_;
  //     return make_lexeme(token_kind::syntax, start);
  //   case '`':
  //     ++it_;
  //     return make_lexeme(token_kind::quasisyntax, start);
  //   case ',':
  //     ++it_;
  //     if (*it_ == '@') {
  //       ++it_;
  //       return make_lexeme(token_kind::unsyntax_splicing, start);
  //     }
  //     return make_lexeme(token_kind::unsyntax, start);
  //   }

  //   return make_lexeme(token_kind::unknown, start);
  // }

  // constexpr value_type lex_string(const char* start) {
  //   assert(*start == '"');

  //   for (auto c = *it_; c != '\0'; c = *++it_) {

  //     if (c == '"') {
  //       ++it_;
  //       return make_lexeme(token_kind::string_lit, start);
  //     }
  //   }

  //   return make_lexeme(token_kind::unterminated_string_lit, start);
  // }

  // constexpr value_type lex_number(const char* start) {
  //   assert(detail::is_digit(*start));

  //   skip_while(detail::is_digit<char>);

  //   if (*it_ == '.') {
  //     ++it_;
  //     skip_while(detail::is_digit<char>);
  //     if (!detail::is_delimiter(*it_)) {
  //       // identifier now
  //       skip_until(detail::is_delimiter<char>);
  //       return make_lexeme(token_kind::identifier, start);
  //     }
  //     return make_lexeme(token_kind::decimal_lit, start);
  //   } else {
  //     if (!detail::is_delimiter(*it_)) {
  //       skip_until(detail::is_delimiter<char>);
  //       return make_lexeme(token_kind::identifier, start);
  //     }

  //     return make_lexeme(token_kind::integer_lit, start);
  //   }
  // }

  // constexpr value_type lex_identifier(const char* start) {
  //   skip_until(detail::is_delimiter<char>);
  //   return make_lexeme(token_kind::identifier, start);
  // }

  // // skips the pred's return while pred(*it_) != 0
  // template <typename P> constexpr void skip_n_while(P pred) {
  //   for (;;) {
  //     auto skip = pred(*it_);
  //     if (skip == 0)
  //       return;
  //     it_ += skip;
  //   }
  // }

  // // skips characters while pred(*it_) is fulfilled
  // template <typename P> constexpr void skip_while(P pred) {
  //   skip_n_while(pred);
  // }

  // template <typename P> constexpr void skip_until(P pred) {
  //   skip_while(std::not_fn<P>(std::move(pred)));
  // }
};
} // namespace stx
} // namespace ely

// template <> struct fmt::formatter<ely::stx::token_kind> {
//   constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin();
//   } template <typename Ctx> constexpr auto format(ely::stx::token_kind k,
//   Ctx& ctx) const {
//     return fmt::format_to(ctx.out(), "{}", ely::stx::token_kind_name(k));
//   }
// };