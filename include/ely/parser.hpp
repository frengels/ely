#pragma once

#include "ely/stx.hpp"
#include "stx.hpp"
#include "tokens.hpp"

namespace ely {
class parser {
public:
  constexpr parser() = default;

  template <typename TokenStream>
  constexpr stx::sexp next(TokenStream& tokens) {
    auto tok = next_skip_atmosphere(tokens);
    assert(!tok.is_atmosphere());

    return next_impl_token(tokens, std::move(tok));
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

  template <typename TokenStream>
  constexpr stx::sexp next_impl_token(TokenStream& tokens, ely::token&& tok) {
    return std::visit(
        [&](auto&& t) -> stx::sexp {
          return next_impl(tokens, static_cast<decltype(t)&&>(t));
        },
        std::move(tok));
  }

  template <typename TokenStream, typename Token>
  constexpr stx::sexp next_impl(TokenStream& tokens, Token&& t) {
    if constexpr (tokens::is_list_start_v<std::remove_cvref_t<Token>>) {
      return parse_list(tokens, static_cast<Token&&>(t));
    } else if constexpr (!tokens::is_atmosphere_v<std::remove_cvref_t<Token>> &&
                         !tokens::is_list_end_v<std::remove_cvref_t<Token>>) {
      return parse(tokens, static_cast<Token&&>(t));
    } else {
      return stx::unknown{};
    }
  }

  template <typename TokenStream, typename LT>
  stx::list parse_list(TokenStream& tokens, LT) {
    std::vector<stx::sexp> elements;

    auto tok = next_skip_atmosphere(tokens);
    assert(!tok.is_atmosphere());

    while (!(tok.is_eof() || tok.template ends_list<LT>())) {
      elements.push_back(next_impl_token(tokens, std::move(tok)));
      tok = next_skip_atmosphere(tokens);
      assert(!tok.is_atmosphere());
    }

    return stx::list(std::move(elements));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unknown) {
    return stx::unknown{};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quote) {
    auto exp = next(tokens);

    if (auto* l = exp.template as<stx::list>()) {
      l->emplace_front(ely::stx::identifier{"quote"});
      return *l;
    } else {
      std::vector<stx::sexp> elements{};
      elements.emplace_back(std::in_place_type<ely::stx::identifier>, "quote");
      elements.emplace_back(std::move(exp));
      return stx::list(std::move(elements));
    }
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unquote) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quasiquote) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unquote_splicing) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::syntax) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quasisyntax) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unsyntax) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unsyntax_splicing) {
    return {};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::identifier&& id) {
    return stx::identifier(std::move(id.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::integer_lit&& ilit) {
    return stx::integer_lit(std::move(ilit.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::decimal_lit&& dlit) {
    return stx::decimal_lit(std::move(dlit.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::string_lit&& str_lit) {
    return stx::string_lit(std::move(str_lit.text));
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens,
                            tokens::unterminated_string_lit&& ustr_lit) {
    return stx::unterminated_string_lit(std::move(ustr_lit.text));
  }

  template <typename TokenStream>
  constexpr stx::eof parse(TokenStream& tokens, const tokens::eof&) {
    return stx::eof{};
  }
};
} // namespace ely