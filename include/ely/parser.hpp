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
    auto tok = tokens.next();

    return std::visit([&](const auto& t) { return next_impl(tokens, t); }, tok);
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
  constexpr stx::sexp next_impl(TokenStream& tokens, const Token& t) {
    if constexpr (detail::is_one_of_v<Token, ely::tokens::lparen,
                                      ely::tokens::lbrace,
                                      ely::tokens::lbracket>) {
      return parse_list(tokens, t);
    } else if constexpr (std::is_same_v<Token, tokens::identifier>) {
      return parse(tokens, t);
    }

    return stx::unknown{};
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
  constexpr std::shared_ptr<stx::identifier>
  parse(TokenStream& tokens, const tokens::identifier& id) {
    return std::make_shared<stx::identifier>(std::move(id.text));
  }
};
} // namespace ely