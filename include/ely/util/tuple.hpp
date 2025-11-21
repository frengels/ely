#pragma once

#include <functional>
#include <tuple>

#include "ely/util/hash.hpp"

namespace ely {
template <typename... Ts> using tuple = std::tuple<Ts...>;

namespace detail {
template <typename TupleT, std::size_t... Is, typename F>
constexpr decltype(auto) apply_helper(TupleT&& ts, std::index_sequence<Is...>,
                                      F&& fn) {
  using std::get;
  return std::invoke(static_cast<F&&>(fn),
                     get<Is>(static_cast<TupleT&&>(ts))...);
}
} // namespace detail

template <typename... Ts, typename F>
constexpr decltype(auto) apply(const ely::tuple<Ts...>& ts, F&& fn) {
  return detail::apply_helper(ts, std::index_sequence_for<Ts...>{},
                              static_cast<F&&>(fn));
}
} // namespace ely

namespace std {
template <typename... Ts> struct hash<ely::tuple<Ts...>> {
  constexpr std::size_t operator()(const ely::tuple<Ts...>& ts) const noexcept {
    return ely::apply(ts, ely::hash_combine);
  }
};
} // namespace std