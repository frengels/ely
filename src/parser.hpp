#pragma once

#include <list>
#include <vector>

#include "lexer.hpp"

namespace ely {
namespace stx {
class sexp {};
class list {};
} // namespace stx

template <typename TokenStream> class parser {
private:
  std::list<std::string> strings_{};
  TokenStream lexer_;

public:
  explicit constexpr parser(const TokenStream& tokens) : lexer_(tokens) {}
  explicit constexpr parser(TokenStream&& tokens) : lexer_(std::move(tokens)) {}

  template <typename Buffer> constexpr stx::sexp* next(Buffer& buf) {
    std::string buffer;

    auto tok = lexer_.next(buf);

    return nullptr;
  }

private:
  template <typename Buffer>
  constexpr stx::list* parse_list(Buffer& buf, token_kind tok) {
    std::vector<stx::sexp*> elements;

    tok = lexer_.next(buf);

    return nullptr;
  }
};
} // namespace ely
