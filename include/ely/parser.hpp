#pragma once

#include "ely/cast.hpp"
#include "ely/stx.hpp"
#include "ely/tokens.hpp"
#include "stx.hpp"

namespace ely {
template <typename TokenStream, typename Interner, typename Arena>
class parser {
private:
  TokenStream* tokens_;
  Arena* arena_;
  Interner* interner_;

public:
  parser() = default;

  constexpr parser(TokenStream& ts, Arena& arena, Interner& interner)
      : tokens_(std::addressof(ts)), arena_(std::addressof(arena)),
        interner_(std::addressof(interner)) {}

  constexpr stx::sexp* next() {
    auto tok = next_skip_atmosphere();
    assert(!tok.is_atmosphere());

    return next_impl_token(std::move(tok));
  }

private:
  constexpr ely::token next_skip_atmosphere() {
    auto tok = tokens_->next();

    while (tok.is_atmosphere()) {
      tok = tokens_->next();
    }

    return tok;
  }

  constexpr stx::sexp* next_impl_token(ely::token&& tok) {
    return std::visit(
        [&](auto&& t) -> stx::sexp* {
          return next_impl(static_cast<decltype(t)&&>(t));
        },
        std::move(tok));
  }

  template <typename Token> constexpr stx::sexp* next_impl(Token&& t) {
    if constexpr (!tokens::is_atmosphere_v<std::remove_cvref_t<Token>> &&
                  !tokens::is_list_end_v<std::remove_cvref_t<Token>>) {
      return parse(static_cast<Token&&>(t));
    } else {
      return stx::sexp::create<stx::unknown>(*arena_);
    }
  }

  constexpr stx::sexp* parse_path() {
    auto tok = next_skip_atmosphere();
    if (auto* s = ely::dyn_cast<tokens::slash>(tok)) {
      return parse_absolute_path(*s);
    }
  }

  constexpr stx::sexp* parse(ely::tokens::lparen) {
    std::vector<stx::sexp*> elements;

    auto tok = next_skip_atmosphere();
    assert(!tok.is_atmosphere());

    while (!(tok.is_eof() || tok.template ends_list<ely::tokens::lparen>())) {
      elements.push_back(next_impl_token(std::move(tok)));
      tok = next_skip_atmosphere();
      assert(!tok.is_atmosphere());
    }

    return stx::sexp::create<stx::list>(*arena_, elements.begin(),
                                        elements.end());
  }

  constexpr stx::sexp* parse(ely::tokens::lbrace) {
    return stx::sexp::create<stx::unknown>(*arena_);
  }

  constexpr stx::sexp* parse(ely::tokens::lbracket) {
    return stx::sexp::create<stx::unknown>(*arena_);
  }

  constexpr stx::sexp* parse(tokens::unknown) {
    return stx::sexp::create<stx::unknown>(*arena_);
  }

  constexpr stx::sexp* parse(tokens::slash) {
    return stx::sexp::create<stx::unknown>(*arena_);
  }

  template <typename Prefix> constexpr stx::sexp* parse_prefix(Prefix) {
    auto* exp = next();
    auto* quote_stx = stx::sexp::create<stx::identifier>(
        *arena_, interner_->intern(Prefix::symbol));

    // if there is a list put `quote` infront '(1 2) -> (quote 1 2)
    if (auto* l = exp->as_list()) {
      l->push_front(quote_stx);
      return exp;
    } else {
      std::vector<stx::sexp*> elements{};
      elements.push_back(quote_stx);
      elements.push_back(exp);
      return stx::sexp::create<stx::list>(*arena_, elements.begin(),
                                          elements.end());
    }
  }

  constexpr stx::sexp* parse(tokens::quote q) { return parse_prefix(q); }

  constexpr stx::sexp* parse(tokens::unquote uq) { return parse_prefix(uq); }

  constexpr stx::sexp* parse(tokens::quasiquote qq) { return parse_prefix(qq); }

  constexpr stx::sexp* parse(tokens::unquote_splicing uqs) {
    return parse_prefix(uqs);
  }

  constexpr stx::sexp* parse(tokens::syntax s) { return parse_prefix(s); }

  constexpr stx::sexp* parse(tokens::quasisyntax qs) {
    return parse_prefix(qs);
  }

  constexpr stx::sexp* parse(tokens::unsyntax us) { return parse_prefix(us); }

  constexpr stx::sexp* parse(tokens::unsyntax_splicing uss) {
    return parse_prefix(uss);
  }

  constexpr stx::sexp* parse(tokens::identifier&& id) {
    // TODO: add origin
    return stx::sexp::create<stx::identifier>(*arena_, id.sym);
  }

  constexpr stx::sexp* parse(tokens::integer_lit&& ilit) {
    // TODO: add origin
    return stx::sexp::create<stx::integer_lit>(*arena_, ilit.text);
  }

  constexpr stx::sexp* parse(tokens::decimal_lit&& dlit) {
    // TODO: add origin
    return stx::sexp::create<stx::decimal_lit>(*arena_, dlit.text);
  }

  constexpr stx::sexp* parse(tokens::string_lit&& str_lit) {
    // TODO: add origin
    return stx::sexp::create<stx::string_lit>(*arena_, str_lit.text);
  }

  constexpr stx::sexp* parse(tokens::unterminated_string_lit&& ustr_lit) {
    // TODO: add origin
    return stx::sexp::create<stx::unterminated_string_lit>(*arena_,
                                                           ustr_lit.text);
  }

  constexpr stx::sexp* parse(const tokens::eof&) {
    // TODO: add origin
    return stx::sexp::create<stx::eof>(*arena_);
  }
};
} // namespace ely