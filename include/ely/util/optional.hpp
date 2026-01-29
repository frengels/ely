#pragma once

#include <functional>
#include <type_traits>
#include <utility>

#include "ely/util/variant.hpp"

namespace ely {
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

  constexpr void swap(nullopt_t&) noexcept {}
};

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

  template <typename Self, typename F>
  constexpr auto transform(this Self&& self, F&& fn) {
    // this is an excellent use of explicit object member functions, eliminating
    // the need for all those error prone overloads
    return just<
        std::invoke_result_t<F, decltype((static_cast<Self&&>(self).val_))>>(
        std::in_place,
        std::invoke(static_cast<F&&>(fn), (static_cast<Self&&>(self).val_)));
  }

  // can't implement `and_then` here as it has to return an optional which isn't
  // defined yet.

  template <typename Self, typename F>
  constexpr decltype(auto) or_else(this Self&& self, const F&) {
    return (static_cast<Self&&>(self).val_);
  }

  constexpr void swap(just& other) noexcept(std::is_nothrow_swappable_v<T>) {
    using std::swap;
    swap(val_, other.val_);
  }
};

inline constexpr auto nullopt = nullopt_t{};

template <typename T> class optional : public ely::variant<nullopt_t, just<T>> {
public:
  using value_type = T;
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

  constexpr bool has_value() const noexcept { return index() != 0; }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  template <typename Self>
  constexpr decltype(auto) operator*(this Self&& self) noexcept {
    return static_cast<Self&&>(self).get_unchecked(std::in_place_index<1>);
  }

  template <typename Self, typename U>
  constexpr T value_or(this Self&& self, U&& default_val) {
    return self.has_value() ? *static_cast<Self&&>(self)
                            : static_cast<T>(static_cast<U&&>(default_val));
  }

  template <typename... Args>
  constexpr T& emplace(Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>) {
    return base::emplace(std::in_place_index<1>, std::in_place,
                         static_cast<Args&&>(args)...);
  }

  constexpr void reset() noexcept { base::emplace(std::in_place_index<0>); }

  template <typename Self, typename F>
  constexpr auto transform(this Self&& self, F&& fn) {
    return ely::visit(
        [&]<typename X>(X&& x) -> decltype(auto) {
          using ret_type =
              std::invoke_result_t<F&&, decltype(*static_cast<Self&&>(self))>;
          // this will return a just or nullopt and construct our optional
          return optional<ret_type>{
              static_cast<X&&>(x).transform(static_cast<F&&>(fn))};
        },
        static_cast<Self&&>(self));
  }

  template <typename Self, typename F>
  constexpr optional<
      std::invoke_result_t<F&&, decltype(*std::declval<Self&&>())>>
  and_then(this Self&& self, F&& fn) {
    if (self.has_value()) {
      return std::invoke(static_cast<F&&>(fn), *static_cast<Self&&>(self));
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

  // TODO: implement swap
};
} // namespace ely