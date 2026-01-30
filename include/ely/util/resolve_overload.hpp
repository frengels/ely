#pragma once

#include <functional>
#include <type_traits>

#include "ely/util/traits.hpp"

namespace ely {
namespace detail {
template <typename...> struct overload;

template <> struct overload<> {
  void operator()() const;
};

template <typename T, typename... Rest>
struct overload<T, Rest...> : overload<Rest...> {
  using overload<Rest...>::operator();
  std::type_identity<T> operator()(T) const;
};
} // namespace detail

template <typename U, typename... Ts>
using resolve_overload_t =
    typename decltype(detail::overload<Ts...>()(std::declval<U>()))::type;

template <typename U, typename... Ts>
inline constexpr std::size_t resolve_overload_index_v =
    ely::find_index_v<resolve_overload_t<U, Ts...>, Ts...>;
} // namespace ely