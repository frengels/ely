#pragma once

#include <algorithm>

namespace ely {
template <typename... Ts> struct first_element;

template <typename T, typename... Rest> struct first_element<T, Rest...> {
  using type = T;
};

template <typename... Ts>
using first_element_t = typename first_element<Ts...>::type;

template <typename T, typename... Ts>
inline constexpr std::size_t find_index_v = [] {
  constexpr bool vs[] = {std::is_same_v<T, Ts>...};
  auto it = std::find(std::begin(vs), std::end(vs), true);
  return std::distance(std::begin(vs), it);
}();

template <std::size_t I, typename... Ts> struct type_pack_element {
  using type = __type_pack_element<I, Ts...>;
};
template <std::size_t I, typename... Ts>
using type_pack_element_t = typename type_pack_element<I, Ts...>::type;
} // namespace ely
