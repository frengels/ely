#pragma once

#include <cstdint>
#include <span>
#include <string_view>

namespace ely {
namespace stx {
std::size_t lex(std::string_view src, std::span<std::uint8_t> out,
                std::uint8_t cont_id = 0);
} // namespace stx
} // namespace ely