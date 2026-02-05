#pragma once

#include "ely/util/dispatch_index.hpp"
#include "ely/util/get_unchecked.hpp"
#include "ely/util/variant_size.hpp"

namespace ely {
template <typename R, typename Visitor, typename Variant>
constexpr R visit(Visitor&& fn, Variant&& v) {
  return ely::dispatch_index_r<
      R, ely::variant_size_v<std::remove_cvref_t<Variant>>>(
      v.index(), [&]<std::size_t I>(std::in_place_index_t<I>) -> R {
        static_assert(
            std::is_same_v<R, std::invoke_result_t<
                                  Visitor&&, decltype(ely::get_unchecked<I>(
                                                 static_cast<Variant&&>(v)))>>,
            "Calling visitor on variant does not return R");
        return std::invoke(static_cast<Visitor&&>(fn),
                           ely::get_unchecked<I>(static_cast<Variant&&>(v)));
      });
}

template <typename Visitor, typename Variant>
constexpr decltype(auto) visit(Visitor&& fn, Variant&& v) {
  using ret_type =
      std::invoke_result_t<Visitor&&, decltype(ely::get_unchecked<0>(
                                          std::declval<Variant&&>()))>;
  return visit<ret_type>(static_cast<Visitor&&>(fn), static_cast<Variant&&>(v));
}

// TODO: implement lazy_visit that allows continuations
} // namespace ely