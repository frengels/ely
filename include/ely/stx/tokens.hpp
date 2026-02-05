#pragma once

#include <cstdint>

namespace ely {
namespace stx {
enum struct token_kind : std::uint8_t {
#define TOKEN(x) x,
#include "tokens.def"
#undef TOKEN
};

constexpr bool ely_token_is_newline(token_kind tk) {
  switch (tk) {
  case token_kind::newline_lf:
  case token_kind::newline_cr:
  case token_kind::newline_crlf:
    return true;
  default:
    return false;
  }
}

constexpr bool ely_token_is_atmosphere(token_kind tk) {
  switch (tk) {
  case token_kind::whitespace:
  case token_kind::tab:
  case token_kind::newline_cr:
  case token_kind::newline_lf:
  case token_kind::newline_crlf:
  case token_kind::line_comment:
  case token_kind::block_comment:
    return true;
  default:
    return false;
  }
}
} // namespace stx
} // namespace ely
