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
    if constexpr (!tokens::is_atmosphere_v<std::remove_cvref_t<Token>> &&
                  !tokens::is_list_end_v<std::remove_cvref_t<Token>>) {
      return parse(tokens, static_cast<Token&&>(t));
    } else {
      return stx::unknown{};
    }
  }

  template <typename TokenStream>
  stx::sexp parse(TokenStream& tokens, ely::tokens::lparen) {
    std::vector<stx::sexp> elements;

    auto tok = next_skip_atmosphere(tokens);
    assert(!tok.is_atmosphere());

    while (!(tok.is_eof() || tok.template ends_list<ely::tokens::lparen>())) {
      elements.push_back(next_impl_token(tokens, std::move(tok)));
      tok = next_skip_atmosphere(tokens);
      assert(!tok.is_atmosphere());
    }

    return stx::sexp(std::in_place_type<stx::list>, std::move(elements));
  }

  template <typename TokenStream>
  stx::sexp parse(TokenStream& tokens, ely::tokens::lbrace) {
    return stx::unknown{};
  }

  template <typename TokenStream>
  stx::sexp parse(TokenStream& tokens, ely::tokens::lbracket) {
    return stx::unknown{};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unknown) {
    return stx::unknown{};
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::slash) {
    return stx::unknown{};
  }

  template <typename TokenStream, typename Prefix>
  constexpr stx::sexp parse_prefix(TokenStream& tokens, Prefix) {
    auto exp = next(tokens);

    if (auto* l = exp.template as<stx::list>()) {
      l->emplace_front(ely::stx::identifier(Prefix::symbol));
      return *l;
    } else {
      std::vector<stx::sexp> elements{};
      elements.emplace_back(std::in_place_type<ely::stx::identifier>,
                            Prefix::symbol);
      elements.emplace_back(std::move(exp));
      return stx::list(std::move(elements));
    }
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quote q) {
    return parse_prefix(tokens, q);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unquote uq) {
    return parse_prefix(tokens, uq);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quasiquote qq) {
    return parse_prefix(tokens, qq);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unquote_splicing uqs) {
    return parse_prefix(tokens, uqs);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::syntax s) {
    return parse_prefix(tokens, s);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::quasisyntax qs) {
    return parse_prefix(tokens, qs);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens, tokens::unsyntax us) {
    return parse_prefix(tokens, us);
  }

  template <typename TokenStream>
  constexpr stx::sexp parse(TokenStream& tokens,
                            tokens::unsyntax_splicing uss) {
    return parse_prefix(tokens, uss);
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