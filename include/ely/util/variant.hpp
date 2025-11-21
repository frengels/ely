#pragma once

#include "ely/util/concepts.hpp"
#include "ely/util/dispatch_index.hpp"
#include "ely/util/hash.hpp"
#include "ely/util/resolve_overload.hpp"
#include "ely/util/traits.hpp"
#include "ely/util/union_storage.hpp"

#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>
#include <utility>

namespace ely {
namespace detail {
class constant_zero {
public:
  constant_zero() = default;
  explicit constexpr constant_zero(std::size_t) {}

  constexpr operator std::size_t() const { return 0; }

  friend constexpr bool operator==(const constant_zero& lhs,
                                   const constant_zero& rhs) {
    return true;
  }
};
} // namespace detail
} // namespace ely

namespace std {
template <> class numeric_limits<::ely::detail::constant_zero> {
public:
  static constexpr bool is_specialized = true;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr int radix = 2;
  static constexpr bool traps = true;

  static constexpr std::size_t min() noexcept { return 0; }
  static constexpr std::size_t max() noexcept { return 0; }
};
} // namespace std

namespace ely {
namespace detail {
template <std::size_t I>
using get_index_type_t = std::conditional_t<
    I == 1, constant_zero,
    std::conditional_t<
        I <= std::numeric_limits<std::uint8_t>::max(), std::uint8_t,
        std::conditional_t<I <= std::numeric_limits<std::uint16_t>::max(),
                           std::uint16_t, std::uint32_t>>>;
} // namespace detail

template <typename... Ts> class variant {
public:
  using index_type = detail::get_index_type_t<sizeof...(Ts)>;

private:
  [[no_unique_address]] union_storage<Ts...> store_;
  [[no_unique_address]] index_type idx_;

public:
  constexpr variant() : store_(std::in_place_index<0>), idx_(0) {}

  template <std::size_t I, typename... Args>
    requires(I < sizeof...(Ts))
  explicit constexpr variant(std::in_place_index_t<I> i, Args&&... args)
      : store_(i, static_cast<Args&&>(args)...), idx_(I) {}

  template <typename T, typename... Args>
    requires(ely::find_index_v<T, Ts...> != sizeof...(Ts) &&
             std::constructible_from<T, Args...>)
  explicit constexpr variant(std::in_place_type_t<T>, Args&&... args)
      : variant(std::in_place_index<ely::find_index_v<T, Ts...>>,
                static_cast<Args&&>(args)...) {}

  template <typename U>
    requires(
        std::constructible_from<ely::resolve_overload_t<U &&, Ts...>, U &&>)
  constexpr variant(U&& u) noexcept(
      std::is_nothrow_constructible_v<ely::resolve_overload_t<U&&, Ts...>>)
      : variant(std::in_place_index<ely::resolve_overload_index_v<U&&, Ts...>>,
                static_cast<U&&>(u)) {}

  constexpr variant(const variant& other) noexcept(
      (std::is_nothrow_copy_constructible_v<Ts> && ...))
    requires(!(std::is_trivially_copy_constructible_v<Ts> && ...))
      : store_([&]() -> union_storage<Ts...> {
          return ely::dispatch_index_r<sizeof...(Ts),
                                       ely::union_storage<Ts...>>(
              other.index(),
              [&]<std::size_t I>(
                  std::in_place_index_t<I> i) -> ely::union_storage<Ts...> {
                return ely::union_storage<Ts...>(i, other.get_unchecked(i));
              });
        }()),
        idx_(other.index()) {}

  variant(const variant&)
    requires((std::is_trivially_copy_constructible_v<Ts> && ...))
  = default;

  variant(const variant&)
    requires(!(std::is_copy_constructible_v<Ts> || ...))
  = delete;

  constexpr variant(variant&& other) noexcept(
      (std::is_nothrow_move_constructible_v<Ts> && ...))
    requires(!(std::is_trivially_move_constructible_v<Ts> && ...))
      : store_([&]() -> union_storage<Ts...> {
          return ely::dispatch_index_r<sizeof...(Ts),
                                       ely::union_storage<Ts...>>(
              other.index(),
              [&]<std::size_t I>(
                  std::in_place_index_t<I> i) -> ely::union_storage<Ts...> {
                return ely::union_storage<Ts...>(
                    i, std::move(other).get_unchecked(i));
              });
        }()),
        idx_(other.index()) {}

  variant(variant&&)
    requires((std::is_trivially_move_constructible_v<Ts> && ...))
  = default;

  variant(variant&&)
    requires(!(std::is_move_constructible_v<Ts> || ...))
  = delete;

  constexpr variant& operator=(const variant& other) noexcept(
      (std::is_nothrow_copy_assignable_v<Ts> && ...))
    requires(!(std::is_trivially_copy_assignable_v<Ts> && ...))
  {
    ely::dispatch_index<sizeof...(Ts)>(
        other.index(), [&]<std::size_t I>(std::in_place_index_t<I> i) {
          this->emplace(i, other);
        });
    return *this;
  }

  variant& operator=(const variant&)
    requires((std::is_trivially_copy_assignable_v<Ts> && ...))
  = default;

  constexpr variant& operator=(variant&& other) noexcept(
      (std::is_nothrow_move_assignable_v<Ts> && ...))
    requires(!(std::is_trivially_move_assignable_v<Ts> && ...))
  {
    ely::dispatch_index<sizeof...(Ts)>(
        other.index(), [&]<std::size_t I>(std::in_place_index_t<I> i) {
          this->emplace(i, std::move(other));
        });
    return *this;
  }

  variant& operator=(variant&& other)
    requires((std::is_trivially_move_assignable_v<Ts> && ...))
  = default;

  constexpr ~variant() noexcept
    requires(!(std::is_trivially_destructible_v<Ts> && ...))
  {
    ely::dispatch_index_r<void, sizeof...(Ts)>(
        index(), [&]<std::size_t I>(std::in_place_index_t<I> i) noexcept {
          std::destroy_at(std::addressof(this->get_unchecked(i)));
        });
  }

  ~variant()
    requires((std::is_trivially_destructible_v<Ts> && ...))
  = default;

  static constexpr std::size_t size() noexcept { return sizeof...(Ts); }

  constexpr std::size_t index() const noexcept {
    return static_cast<std::size_t>(idx_);
  }

  friend constexpr bool operator==(const variant& lhs,
                                   const variant& rhs) noexcept {
    if (lhs.index() != rhs.index()) {
      return false;
    }

    return ely::dispatch_index_r<bool, sizeof...(Ts)>(
        lhs.index(), [&]<std::size_t I>(std::in_place_index_t<I> i) -> bool {
          const auto& lhs_x = lhs.get_unchecked(i);
          const auto& rhs_x = rhs.get_unchecked(i);
          return lhs_x == rhs_x;
        });
  }

  template <ely::any_of<Ts...> T>
  friend constexpr bool operator==(const variant& lhs, const T& rhs) {
    constexpr auto idx = ely::find_index_v<T, Ts...>;
    if (lhs.index() != idx) {
      return false;
    }

    return lhs.get_unchecked(std::in_place_index<idx>) == rhs;
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr auto& get_unchecked(std::in_place_index_t<I> i) & noexcept {
    assert(I == index());
    return store_.get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr const auto&
  get_unchecked(std::in_place_index_t<I> i) const& noexcept {
    assert(I == index());
    return store_.get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr auto&& get_unchecked(std::in_place_index_t<I> i) && noexcept {
    assert(I == index());
    return std::move(store_).get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr const auto&&
  get_unchecked(std::in_place_index_t<I> i) const&& noexcept {
    assert(I == index());
    return std::move(store_).get(i);
  }

  template <ely::any_of<Ts...> T>
  constexpr auto& get_unchecked(std::in_place_type_t<T>) & noexcept {
    return get_unchecked(std::in_place_index<ely::find_index_v<T, Ts...>>);
  }

  template <ely::any_of<Ts...> T>
  constexpr const auto& get_unchecked(std::in_place_type_t<T>) const& noexcept {
    return get_unchecked(std::in_place_index<ely::find_index_v<T, Ts...>>);
  }

  template <ely::any_of<Ts...> T>
  constexpr auto&& get_unchecked(std::in_place_type_t<T>) && noexcept {
    return std::move(*this).get_unchecked(
        std::in_place_index<ely::find_index_v<T, Ts...>>);
  }

  template <ely::any_of<Ts...> T>
  constexpr const auto&&
  get_unchecked(std::in_place_type_t<T>) const&& noexcept {
    return std::move(*this).get_unchecked(
        std::in_place_index<ely::find_index_v<T, Ts...>>);
  }

  template <std::size_t I, typename... Args>
  constexpr auto& emplace(std::in_place_index_t<I> i, Args&&... args) {
    // implement destroy here, as it's not useful outside of this interface
    ely::dispatch_index<sizeof...(Ts)>(
        index(), [&]<std::size_t J>(std::in_place_index_t<J> j) {
          this->store_.destroy(j);
        });
    store_.emplace(i, static_cast<Args&&>(args)...);
    idx_ = I;
    return get_unchecked(i);
  }
};
} // namespace ely
