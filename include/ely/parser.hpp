#pragma once

#include "ely/stx.hpp"
#include "stx.hpp"
#include "tokens.hpp"

namespace ely {
namespace detail {
template <typename T, typename... Ts>
constexpr bool is_one_of_v = (std::is_same_v<T, Ts> || ...);
}

class parser {
public:
  constexpr parser() = default;

  template <typename TokenStream>
  constexpr stx::sexp next(TokenStream& tokens) {
    auto tok = next_skip_atmosphere(tokens);
    assert(!tok.is_atmosphere());

    return std::visit([&](auto& t) { return next_impl(tokens, t); }, tok);
  }

private:
  template <typename TokenStream>
  constexpr ely::token next_skip_atmosphere(TokenStream& tokens) {
    auto tok = tokens.next();

    while (tok.is_atmosphere()) {
      tok = tokens.next();
    }

    return tok;
  }

  template <typename TokenStream, typename Token>
  void next_impl(TokenStream&, const Token&) = delete;

  template <typename TokenStream>
  constexpr stx::sexp next_impl(TokenStream& tokens, ely::token& tok) {
    return std::visit(
        [&](auto& t) -> stx::sexp { return next_impl(tokens, t); }, tok);
  }

  template <typename TokenStream, typename Token>
  constexpr stx::sexp next_impl(TokenStream& tokens, Token& t) {
    if constexpr (detail::is_one_of_v<Token, ely::tokens::lparen,
                                      ely::tokens::lbrace,
                                      ely::tokens::lbracket>) {
      return parse_list(tokens, t);
    } else if constexpr (detail::is_one_of_v<
                             Token, ely::tokens::identifier,
                             ely::tokens::integer_lit, ely::tokens::decimal_lit,
                             ely::tokens::string_lit, ely::tokens::eof>) {
      return parse(tokens, std::move(t));
    } else {
      return stx::unknown{};
    }
  }

  template <typename TokenStream, typename LT>
  constexpr std::shared_ptr<stx::list> parse_list(TokenStream& tokens, LT) {
    std::vector<stx::sexp> elements;

    auto tok = next_skip_atmosphere(tokens);
    assert(!tok.is_atmosphere());

    while (!(tok.is_eof() || tok.template ends_list<LT>())) {
      elements.push_back(next_impl(tokens, tok));
      tok = next_skip_atmosphere(tokens);
      assert(!tok.is_atmosphere());
    }

    return std::make_shared<stx::list>(std::move(elements));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::identifier&& id) {
    return std::make_shared<stx::identifier>(std::move(id.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::integer_lit&& ilit) {
    return std::make_shared<stx::integer_lit>(std::move(ilit.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::decimal_lit&& dlit) {
    return std::make_shared<stx::decimal_lit>(std::move(dlit.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::string_lit&& str_lit) {
    return std::make_shared<stx::string_lit>(std::move(str_lit.text));
  }

  template <typename TokenStream>
  constexpr stx::eof parse(TokenStream& tokens, const tokens::eof&) {
    return stx::eof{};
  }

  template <typename TokenStream>
  constexpr stx::unknown parse(TokenStream& tokens, const tokens::unknown&) {
    return stx::unknown{};
  }
};
} // namespace ely