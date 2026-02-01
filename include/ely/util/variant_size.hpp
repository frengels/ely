#pragma once

#include "ely/util/variant.hpp"

#include <variant>

namespace ely {

namespace detail {
// used for derived types
template <typename... Ts>
auto as_variant(const ely::variant<Ts...>&) -> ely::variant<Ts...>;

template <typename... Ts>
auto as_variant(const std::variant<Ts...>&) -> std::variant<Ts...>;
} // namespace detail

template <typename> struct variant_size {};

template <typename V>
  requires requires(const V& v) { detail::as_variant(v); }
struct variant_size<V>
    : variant_size<decltype(detail::as_variant(std::declval<const V&>()))> {};

template <typename... Ts>
struct variant_size<ely::variant<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <typename T>
  requires requires { std::variant_size<T>::value; }
struct variant_size<T> : std::variant_size<T> {};

template <typename Variant>
inline constexpr std::size_t variant_size_v = variant_size<Variant>::value;
} // namespace ely
