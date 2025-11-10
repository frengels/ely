#pragma once

namespace ely {
template <typename... Ts> struct first_element;

template <typename T, typename... Rest> struct first_element<T, Rest...> {
  using type = T;
};

template <typename... Ts>
using first_element_t = typename first_element<Ts...>::type;
} // namespace ely
