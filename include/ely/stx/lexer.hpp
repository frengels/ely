#pragma once

#include <cstdint>
#include <span>
#include <string_view>

#include "ely/config.h"

namespace detail {
extern "C" std::size_t ely_lex(const char* src, std::size_t src_len,
                               std::uint8_t* out_buffer,
                               std::size_t out_buffer_len,
                               std::uint8_t cont_id);
}

namespace ely {
namespace stx {
ELY_ALWAYS_INLINE std::size_t lex(std::string_view src,
                                  std::span<std::uint8_t> out,
                                  std::uint8_t cont_id = 0) {
  return detail::ely_lex(src.data(), src.size(), out.data(), out.size(),
                         cont_id);
}
} // namespace stx
} // namespace ely