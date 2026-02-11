#pragma once

#include "ely/util/concepts.hpp"
#include "ely/util/variant.hpp"

#include <utility>
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

template <typename T, typename... Ts>
  requires(ely::any_of<T, Ts...>)
constexpr auto& get_unchecked(ely::variant<Ts...>& v) noexcept {
  return v.get_unchecked(std::in_place_type<T>);
}

template <typename T, typename... Ts>
  requires(ely::any_of<T, Ts...>)
constexpr const auto& get_unchecked(const ely::variant<Ts...>& v) noexcept {
  return v.get_unchecked(std::in_place_type<T>);
}

template <typename T, typename... Ts>
  requires(ely::any_of<T, Ts...>)
constexpr auto&& get_unchecked(ely::variant<Ts...>&& v) noexcept {
  return std::move(v).get_unchecked(std::in_place_type<T>);
}

template <typename T, typename... Ts>
  requires(ely::any_of<T, Ts...>)
constexpr const auto&& get_unchecked(const ely::variant<Ts...>&& v) noexcept {
  return std::move(v).get_unchecked(std::in_place_type<T>);
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(std::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(v);
  } catch (...) {
    std::unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(const std::variant<Ts...>& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(v);
  } catch (...) {
    std::unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(std::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(std::move(v));
  } catch (...) {
    std::unreachable();
  }
}

template <std::size_t I, typename... Ts>
  requires(I < sizeof...(Ts))
constexpr auto& get_unchecked(const std::variant<Ts...>&& v) noexcept {
  assert(v.index() == I);
  try {
    return std::get<I>(std::move(v));
  } catch (...) {
    std::unreachable();
  }
}
} // namespace ely
