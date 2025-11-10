#pragma once

#include "ely/util/variant.hpp"

#include <variant>

namespace ely {
template <typename> struct variant_size {};

template <typename... Ts>
struct variant_size<ely::variant<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <typename T>
  requires requires { std::variant_size<T>::value; }
struct variant_size<T> : std::variant_size<T> {};

template <typename Variant>
inline constexpr std::size_t variant_size_v = variant_size<Variant>::value;
} // namespace ely
