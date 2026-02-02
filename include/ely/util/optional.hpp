#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "ely/util/variant.hpp"

namespace ely {
template <typename T> class optional;

namespace detail {
template <typename> struct is_optional : std::false_type {};
template <typename T> struct is_optional<ely::optional<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<T>::value;

template <typename F, typename T>
struct invoke_result_no_void : std::invoke_result<F, T> {};
template <typename F>
struct invoke_result_no_void<F, void> : std::invoke_result<F> {};
template <typename F, typename T>
using invoke_result_no_void_t = typename invoke_result_no_void<F, T>::type;
} // namespace detail
struct nullopt_t {
  nullopt_t() = default;

  template <typename F> constexpr nullopt_t transform(const F&) const {
    return *this;
  }

  template <typename F> constexpr nullopt_t and_then(const F&) const {
    return *this;
  }

  template <typename F> constexpr decltype(auto) or_else(F&& fn) const {
    return std::invoke(static_cast<F&&>(fn));
  }

  template <typename F> constexpr decltype(auto) value_or_else(F&& fn) const {
    return std::invoke(static_cast<F&&>(fn));
  }

  constexpr void swap(nullopt_t&) noexcept {}
};

template <typename T> class just;

namespace detail {
template <typename> struct is_just : std::false_type {};
template <typename T> struct is_just<ely::just<T>> : std::true_type {};

template <typename T> inline constexpr bool is_just_v = is_just<T>::value;
} // namespace detail

template <typename T> class just {
private:
  [[no_unique_address]] T val_;

public:
  just() = default;

  template <typename... Args>
  constexpr just(std::in_place_t, Args&&... args)
      : val_(static_cast<Args&&>(args)...) {}

  template <typename U>
    requires(std::is_constructible_v<T, U> &&
             !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>)
  explicit(!std::is_convertible_v<U, T>) constexpr just(U&& u)
      : val_(static_cast<U&&>(u)) {}

  template <typename Self> constexpr decltype(auto) value(this Self&& self) {
    return (static_cast<Self&&>(self).val_);
  }

  template <typename Self, typename F>
  constexpr auto transform(this Self&& self, F&& fn) {
    // this is an excellent use of explicit object member functions, eliminating
    // the need for all those error prone overloads
    return just<
        std::invoke_result_t<F, decltype((static_cast<Self&&>(self).val_))>>(
        std::in_place,
        std::invoke(static_cast<F&&>(fn), (static_cast<Self&&>(self).val_)));
  }

  template <typename Self, typename F>
    requires(detail::is_just_v<
             std::invoke_result_t<F, decltype((std::declval<Self &&>().val_))>>)
  constexpr auto and_then(this Self&& self, F&& fn) {
    return std::invoke(static_cast<F&&>(fn), (static_cast<Self&&>(self).val_));
  }

  template <typename Self, typename F>
  constexpr decltype(auto) or_else(this Self&& self, const F&) {
    return (static_cast<Self&&>(self).val_);
  }

  template <typename Self, typename F>
  constexpr T value_or_else(this Self&& self, const F&) {
    return (static_cast<Self&&>(self).val_);
  }

  constexpr void swap(just& other) noexcept(std::is_nothrow_swappable_v<T>) {
    using std::swap;
    swap(val_, other.val_);
  }
};

template <> class just<void> {
public:
  just() = default;

  constexpr just(std::in_place_t) {}

  template <std::invocable F> constexpr auto transform(F&& fn) const {
    return just<std::invoke_result_t<F>>(std::invoke(static_cast<F&&>(fn)));
  }

  template <std::invocable F>
    requires(detail::is_just_v<std::invoke_result_t<F>>)
  constexpr auto and_then(F&& fn) const {
    return std::invoke(static_cast<F&&>(fn));
  }
};

inline constexpr auto nullopt = nullopt_t{};

template <typename T> class optional : public ely::variant<nullopt_t, just<T>> {
public:
  using value_type = T;
  // using add_lvalue_reference keeps void void
  using reference = std::add_lvalue_reference_t<value_type>;
  using base = ely::variant<nullopt_t, just<T>>;

public:
  optional() = default;

  constexpr optional(ely::nullopt_t) : optional() {}

  constexpr optional(const ely::just<T>& ref) : base(ref) {}
  constexpr optional(ely::just<T>&& ref) : base(std::move(ref)) {}

  template <typename U>
    requires(std::is_constructible_v<T, U> &&
             !(std::is_same_v<std::remove_cvref_t<U>, std::in_place_t> ||
               std::is_same_v<std::remove_cvref_t<U>, ely::optional<T>> ||
               std::is_same_v<std::remove_cvref_t<U>, ely::just<T>>))
  // TODO: check for T == bool => U != optional specialization due to operator
  // bool
  explicit(!std::is_convertible_v<U, T>) constexpr optional(U&& u)
      : base(std::in_place_index<1>, static_cast<U&&>(u)) {}

  constexpr bool has_value() const noexcept { return base::index() != 0; }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  template <typename Self>
  constexpr decltype(auto) operator*(this Self&& self) noexcept {
    return static_cast<Self&&>(self)
        .get_unchecked(std::in_place_index<1>)
        .value();
  }

  template <typename Self, typename U = std::remove_cvref_t<T>>
  constexpr T value_or(this Self&& self, U&& default_val) {
    return self.has_value() ? *static_cast<Self&&>(self)
                            : static_cast<T>(static_cast<U&&>(default_val));
  }

  template <typename... Args>
    requires(!std::is_void_v<T>)
  constexpr reference emplace(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>) {
    return base::emplace(std::in_place_index<1>, std::in_place,
                         static_cast<Args&&>(args)...);
  }

  constexpr void reset() noexcept { base::emplace(std::in_place_index<0>); }

  template <typename Self, typename F>
  constexpr auto transform(this Self&& self, F&& fn) {
    using ret_type = detail::invoke_result_no_void_t<F, T>;
    return ely::visit(
        [&]<typename X>(X&& x) -> ely::optional<ret_type> {
          // this will return a just or nullopt and construct our optional
          return static_cast<X&&>(x).transform(static_cast<F&&>(fn));
        },
        static_cast<Self&&>(self));
  }

  template <typename Self, typename F>
    requires(detail::is_optional_v<
             std::invoke_result_t<F, decltype(*std::declval<Self>())>>)
  constexpr std::invoke_result_t<F&&, decltype(*std::declval<Self&&>())>
  and_then(this Self&& self, F&& fn) {
    if (self.has_value()) {
      if constexpr (std::is_void_v<value_type>) {
        return std::invoke(static_cast<F&&>(fn));
      } else {
        return std::invoke(static_cast<F&&>(fn), (*static_cast<Self&&>(self)));
      }
    } else {
      return ely::nullopt;
    }
  }

  template <typename Self, typename F>
  constexpr optional or_else(this Self&& self, F&& fn) {
    return ely::visit(
        [&]<typename X>(X&& x) -> optional {
          return optional{static_cast<X&&>(x).or_else(static_cast<F&&>(fn))};
        },
        static_cast<Self&&>(self));
  }

  // implemented from p3413r0 (https://github.com/cplusplus/papers/issues/2084)
  template <typename Self, typename F>
  constexpr T value_or_else(this Self&& self, F&& fn) {
    return ely::visit(
        [&]<typename X>(X&& x) -> T {
          return T{static_cast<X&&>(x).value_or_else(static_cast<F&&>(fn))};
        },
        static_cast<Self&&>(self));
  }

  // TODO: implement swap
};
} // namespace ely