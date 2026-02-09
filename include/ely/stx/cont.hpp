#pragma once

#include <cstdint>

namespace ely {
namespace stx {
enum struct cont : std::uint8_t {
  start,
  whitespace,
  tab,
  newline_cr,
  identifier,
  decimal_lit,
  integer_lit,
  string_lit,
  line_comment,
  line_comment_cr,
  number_sign,
  unsyntax_splicing,
  unicode4,
  unicode3,
  unicode2,
  last = unicode2,
};
}
} // namespace ely
