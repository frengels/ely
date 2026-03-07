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
      storage_.emplace(std::in_place_type<constexpr_type>);
    } else {
      storage_.emplace(std::in_place_type<runtime_type>);
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

  constexpr bool is_constexpr() const { return std::is_constant_evaluated(); }

  constexpr bool is_runtime() const { return !is_constexpr(); }

  template <typename Self> constexpr decltype(auto) get(this Self&& self) {
    if (self.is_constexpr()) {
      return static_cast<Self&&>(self).storage_.get(
          std::in_place_type<constexpr_type>);
    } else {
      return static_cast<Self&&>(self).storage_.get(
          std::in_place_type<runtime_type>);
    }
  }

  template <typename Self, typename CxFn, typename RtFn>
  constexpr decltype(auto) visit(this Self&& self, CxFn&& cx_fn, RtFn&& rt_fn) {
    if (self.is_constexpr()) {
      return static_cast<CxFn&&>(cx_fn)(static_cast<Self&&>(self).storage_.get(
          std::in_place_type<constexpr_type>));
    } else {
      return std::forward<RtFn>(rt_fn)(static_cast<Self&&>(self).storage_.get(
          std::in_place_type<runtime_type>));
    }
  }

  template <typename Self, typename Fn>
  constexpr decltype(auto) visit(this Self&& self, Fn&& fn) {
    if (self.is_constexpr()) {
      return static_cast<Fn&&>(fn)(static_cast<Self&&>(self).storage_.get(
          std::in_place_type<constexpr_type>));
    } else {
      return std::forward<Fn>(fn)(static_cast<Self&&>(self).storage_.get(
          std::in_place_type<runtime_type>));
    }
  }
};
} // namespace ely
