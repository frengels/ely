#pragma once

#include <string_view>

#include "ely/util/isa.hpp"
#include "ely/util/variant.hpp"

namespace ely {
namespace stx {

enum struct token_kind {
#define TOKEN(x) #x,
#include "tokens.def"
#undef TOKEN
};

constexpr bool token_is_whitespace(token_kind tk) {
  return tk == token_kind::whitespace;
}

constexpr bool token_is_tab(token_kind tk) { return tk == token_kind::tab; }

constexpr bool token_is_newline(token_kind tk) {
  switch (tk) {
  case token_kind::newline_lf:
  case token_kind::newline_cr:
  case token_kind::newline_crlf:
    return true;
  default:
    return false;
  }
}

constexpr bool token_is_line_comment(token_kind tk) {
  return tk == token_kind::line_comment;
}

constexpr bool token_is_block_comment(token_kind) {
  return false; // TODO
}

constexpr bool token_is_comment(token_kind tk) {
  return token_is_line_comment(tk) || token_is_block_comment(tk);
}

constexpr bool token_is_atmosphere(token_kind tk) {
  return token_is_whitespace(tk) || token_is_tab(tk) || token_is_newline(tk) ||
         token_is_comment(tk);
}

constexpr bool token_is_identifier(token_kind tk) {
  return tk == token_kind::identifier;
}

constexpr bool token_is_spill(token_kind tk) { return tk == token_kind::spill; }

constexpr bool token_is_eof(token_kind tk) { return tk == token_kind::eof; }
} // namespace stx
} // namespace ely