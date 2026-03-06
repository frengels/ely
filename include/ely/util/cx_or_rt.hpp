#pragma once

#include "ely/util/union_storage.hpp"

#include <tuple>
#include <type_traits>

namespace ely {
template <typename CxT, typename RtT> struct cx_or_rt {
  using constexpr_type = CxT;
  using runtime_type = RtT;

  static_assert(sizeof(constexpr_type) <= sizeof(runtime_type),
                "the constexpr type shouldn't cause runtime overhead");
  static_assert(alignof(constexpr_type) <= alignof(runtime_type),
                "the constexpr type shouldn't cause runtime overhead");

private:
  [[no_unique_address]] ely::union_storage<char, constexpr_type, runtime_type>
      storage_;

public:
  constexpr cx_or_rt()
    requires(std::is_default_constructible_v<constexpr_type> &&
             std::is_default_constructible_v<runtime_type>)
      : storage_() {
    if consteval {
      storage_.template emplace<constexpr_type>();
    } else {
      storage_.template emplace<runtime_type>();
    }
  }

  template <typename... CxArgs, typename... RtArgs>
    requires(std::constructible_from<constexpr_type, CxArgs...> &&
             std::constructible_from<runtime_type, RtArgs...>)
  explicit constexpr cx_or_rt(std::tuple<CxArgs...> cx_args,
                              std::tuple<RtArgs...> rt_args)
      : storage_() {
    if consteval {
      std::apply(
          [this](auto&&... args) {
            storage_.emplace(std::in_place_type<constexpr_type>,
                             static_cast<CxArgs&&>(args)...);
          },
          cx_args);
    } else {
      std::apply(
          [this](auto&&... args) {
            storage_.emplace(std::in_place_type<runtime_type>,
                             static_cast<RtArgs&&>(args)...);
          },
          rt_args);
    }
  }

  constexpr ~cx_or_rt() noexcept {
    if (is_constexpr()) {
      storage_.destroy(std::in_place_type<constexpr_type>);
    } else {
      storage_.destroy(std::in_place_type<runtime_type>);
    }
  }

  [[nodiscard]] constexpr bool is_constexpr() const {
    return std::is_constant_evaluated();
  }

  [[nodiscard]] constexpr bool is_runtime() const { return !is_constexpr(); }

  [[nodiscard]] constexpr const auto& get() const& {
    return is_constexpr() ? storage_.get(std::in_place_type<constexpr_type>)
                          : storage_.get(std::in_place_type<runtime_type>);
  }

  [[nodiscard]] constexpr auto& get() & {
    return is_constexpr() ? storage_.get(std::in_place_type<constexpr_type>)
                          : storage_.get(std::in_place_type<runtime_type>);
  }

  [[nodiscard]] constexpr auto&& get() && {
    return is_constexpr()
               ? std::move(storage_).get(std::in_place_type<constexpr_type>())
               : std::move(storage_).get(std::in_place_type<runtime_type>());
  }

  [[nodiscard]] constexpr const auto&& get() const&& {
    return is_constexpr()
               ? std::move(storage_).get(std::in_place_type<constexpr_type>())
               : std::move(storage_).get(std::in_place_type<runtime_type>());
  }

  template <typename CxFn, typename RtFn>
  [[nodiscard]] constexpr decltype(auto) visit(CxFn&& cx_fn,
                                               RtFn&& rt_fn) const {
    if (is_constexpr()) {
      return std::forward<CxFn>(cx_fn)(
          storage_.get(std::in_place_type<constexpr_type>()));
    } else {
      return std::forward<RtFn>(rt_fn)(
          storage_.get(std::in_place_type<runtime_type>()));
    }
  }
};
} // namespace ely
