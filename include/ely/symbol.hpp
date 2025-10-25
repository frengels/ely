#pragma once

#include <cassert>
#include <cstdint>
#include <fmt/base.h>
#include <functional>
#include <limits>

#include <fmt/core.h>

namespace ely {
struct symbol {
  using id_type = std::uint32_t;

  id_type id;

  static constexpr auto invalid = std::numeric_limits<decltype(id)>::min();

  constexpr symbol() : symbol(invalid) {}
  explicit constexpr symbol(id_type id) : id(id) {
    assert(id != invalid && "Use default constructor for invalid symbol");
  }

  friend constexpr bool operator==(const symbol& lhs, const symbol& rhs) {
    return lhs.id == rhs.id;
  }
};
} // namespace ely

template <> struct std::hash<ely::symbol> {
  static constexpr std::size_t operator()(const ely::symbol& sym) {
    auto h = std::hash<typename ely::symbol::id_type>{};
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
