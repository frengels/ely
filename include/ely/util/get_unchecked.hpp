#pragma once

#include "ely/util/variant.hpp"

#include <variant>

namespace ely {
template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(ely::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  return v.get_unchecked(std::in_place_index<I>);
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr const auto& get_unchecked(const ely::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  return v.get_unchecked(std::in_place_index<I>);
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto&& get_unchecked(ely::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  return std::move(v).get_unchecked(std::in_place_index<I>);
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr const auto&& get_unchecked(const ely::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  return std::move(v).get_unchecked(std::in_place_index<I>);
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(std::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(v);
  } catch (...) {
    __builtin_unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(const std::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(v);
  } catch (...) {
    __builtin_unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(std::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(std::move(v));
  } catch (...) {
    __builtin_unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(const std::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(std::move(v));
  } catch (...) {
    __builtin_unreachable();
  }
}
} // namespace ely
