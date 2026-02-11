#pragma once

#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

namespace ely {
template <typename T, typename... Ts>
constexpr bool isa(const ely::variant<Ts...>& v) {
  return ely::visit([]<typename U>(U&) { return std::is_same_v<U, T>; },
                    const_cast<ely::variant<Ts...>&>(v));
}

template <typename... Us, typename... Ts>
constexpr bool isa(const ely::variant<Ts...>& v) {
  return ely::visit(
      []<typename U>(U&) { return (std::is_same_v<U, Us> || ...); },
      const_cast<ely::variant<Ts...>&>(v));
}
} // namespace ely
