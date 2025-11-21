#pragma once

#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

// due to the disjointed nature of all the implementations and circular
// dependencies this has to be defined in its own file

namespace std {
template <typename... Variants> struct hash<ely::variant<Variants...>> {
  constexpr std::size_t
  operator()(const ely::variant<Variants...>& v) const noexcept {
    return ely::visit(
        [&]<typename T>(const T& t) -> std::size_t {
          return ely::hash_combine(v.index(), t);
        },
        v);
  }
};
} // namespace std
