#pragma once

#include <ely/config.h>

#include <functional>
#include <type_traits>
#include <utility>

namespace ely {
namespace detail {
template <std::size_t I, std::size_t Max, typename Fn, typename Ret>
  requires(I < Max)
ELY_ALWAYS_INLINE constexpr Ret dispatch_on(Fn&& fn) noexcept(
    std::is_nothrow_invocable_r_v<Ret, Fn&&, std::in_place_index_t<I>>) {
  return std::invoke(static_cast<Fn&&>(fn), std::in_place_index<I>);
}

template <std::size_t I, std::size_t Max, typename Fn, typename Ret>
  requires(I >= Max)
ELY_ALWAYS_INLINE constexpr Ret dispatch_on(Fn&& fn) noexcept {
  // these 2 cases cannot be handled by if constexpr because the invalid variant
  // would still be semantically analyzed
  std::unreachable();
}

template <std::size_t Total, std::size_t Offset, typename Fn, typename Ret>
ELY_ALWAYS_INLINE constexpr Ret dispatch_index_impl(std::size_t index,
                                                    Fn&& fn) {
  if constexpr (Offset > Total) {
    std::unreachable();
  } else {
    if (index >= Total) {
      std::unreachable();
    }

#define DISPATCH_CASE(idx)                                                     \
  case (idx):                                                                  \
    return dispatch_on<(idx), Total, Fn, Ret>(static_cast<Fn&&>(fn))
    switch (index) {
      DISPATCH_CASE(0 + Offset);
      DISPATCH_CASE(1 + Offset);
      DISPATCH_CASE(2 + Offset);
      DISPATCH_CASE(3 + Offset);
      DISPATCH_CASE(4 + Offset);
      DISPATCH_CASE(5 + Offset);
      DISPATCH_CASE(6 + Offset);
      DISPATCH_CASE(7 + Offset);
      DISPATCH_CASE(8 + Offset);
      DISPATCH_CASE(9 + Offset);
      DISPATCH_CASE(10 + Offset);
      DISPATCH_CASE(11 + Offset);
      DISPATCH_CASE(12 + Offset);
      DISPATCH_CASE(13 + Offset);
      DISPATCH_CASE(14 + Offset);
      DISPATCH_CASE(15 + Offset);
#undef DISPATCH_CASE
    default:
      return dispatch_index_impl<Total, Offset + 16, Fn, Ret>(
          index, static_cast<Fn&&>(fn));
    }
  }
}

template <typename Fn, std::size_t... Is>
constexpr bool
all_return_types_same(std::integer_sequence<std::size_t, Is...>) {
  if constexpr (sizeof...(Is) == 0) {
    return true;
  } else {
    using T0 = std::invoke_result_t<Fn&&, std::in_place_index_t<0>>;
    return (
        std::is_same_v<T0,
                       std::invoke_result_t<Fn&&, std::in_place_index_t<Is>>> &&
        ...);
  }
}
} // namespace detail

template <typename Ret, std::size_t Total, typename Fn>
ELY_ALWAYS_INLINE constexpr Ret dispatch_index_r(std::size_t index, Fn&& fn) {
  return detail::dispatch_index_impl<Total, 0, Fn, Ret>(index,
                                                        static_cast<Fn&&>(fn));
}

template <std::size_t Total, typename Fn>
ELY_ALWAYS_INLINE constexpr std::invoke_result_t<Fn&&, std::in_place_index_t<0>>
dispatch_index(std::size_t index, Fn&& fn) {
  static_assert(
      detail::all_return_types_same<Fn&&>(std::make_index_sequence<Total>{}),
      "All return types must match");
  return ely::dispatch_index_r<
      std::invoke_result_t<Fn&&, std::in_place_index_t<0>>, Total>(
      index, static_cast<Fn&&>(fn));
}
} // namespace ely