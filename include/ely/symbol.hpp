#pragma once

#include <cstdint>
#include <fmt/base.h>
#include <functional>
#include <limits>

#include <fmt/core.h>

namespace ely {
struct symbol {
  std::uint32_t id;

  static constexpr auto invalid = std::numeric_limits<decltype(id)>::min();

  explicit constexpr symbol(std::uint32_t id) : id(id) {}

  friend constexpr bool operator==(const symbol& lhs, const symbol& rhs) {
    return lhs == rhs;
  }
};
} // namespace ely

template <> struct std::hash<ely::symbol> {
  static constexpr std::size_t operator()(const ely::symbol& sym) {
    auto h = std::hash<std::size_t>{};
    return h(sym.id);
  }
};

template <> struct fmt::formatter<ely::symbol> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::symbol& sym, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "symbol({})", sym.id);
  }
};
