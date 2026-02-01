#pragma once

#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace ely {

namespace detail {

template <typename T0 = char, typename T1 = T0, typename T2 = T0,
          typename T3 = T0, typename... Rest>
union union_storage_impl {
private:
  inline static constexpr bool has_tail = sizeof...(Rest) > 0;
  using tail_type =
      std::conditional_t<has_tail, union_storage_impl<Rest...>, T3>;

private:
  T0 t0_;
  T1 t1_;
  T2 t2_;
  T3 t3_;
  tail_type rest_;

public:
  template <typename... Args>
  explicit constexpr union_storage_impl(std::in_place_index_t<0>,
                                        Args&&... args)
      : t0_(static_cast<Args&&>(args)...) {}

  template <typename... Args>
  explicit constexpr union_storage_impl(std::in_place_index_t<1>,
                                        Args&&... args)
      : t1_(static_cast<Args&&>(args)...) {}

  template <typename... Args>
  explicit constexpr union_storage_impl(std::in_place_index_t<2>,
                                        Args&&... args)
      : t2_(static_cast<Args&&>(args)...) {}

  template <typename... Args>
  explicit constexpr union_storage_impl(std::in_place_index_t<3>,
                                        Args&&... args)
      : t3_(static_cast<Args&&>(args)...) {}

  template <std::size_t I, typename... Args>
  explicit constexpr union_storage_impl(std::in_place_index_t<I>,
                                        Args&&... args)
      : rest_(std::in_place_index<I - 4>, static_cast<Args&&>(args)...) {}

  constexpr ~union_storage_impl()
    requires(!(std::is_trivially_destructible_v<T0> &&
               std::is_trivially_destructible_v<T1> &&
               std::is_trivially_destructible_v<T2> &&
               std::is_trivially_destructible_v<T3> &&
               std::is_trivially_destructible_v<tail_type>))
  {}

  ~union_storage_impl()
    requires(std::is_trivially_destructible_v<T0> &&
             std::is_trivially_destructible_v<T1> &&
             std::is_trivially_destructible_v<T2> &&
             std::is_trivially_destructible_v<T3> &&
             std::is_trivially_destructible_v<tail_type>)
  = default;

  template <std::size_t I, typename... Args>
  constexpr void emplace(std::in_place_index_t<I> i, Args&&... args) {
    std::construct_at(std::addressof(get(i)), static_cast<Args&&>(args)...);
  }

  template <std::size_t I>
  constexpr void destroy(std::in_place_index_t<I> i) noexcept {
    std::destroy_at(std::addressof(get(i)));
  }

  template <std::size_t I>
  constexpr const auto& get(std::in_place_index_t<I>) const& noexcept {
    if constexpr (I == 0) {
      return static_cast<const T0&>(t0_);
    } else if constexpr (I == 1) {
      return static_cast<const T1&>(t1_);
    } else if constexpr (I == 2) {
      return static_cast<const T2&>(t2_);
    } else if constexpr (I == 3) {
      return static_cast<const T3&>(t3_);
    } else {
      return rest_.get(std::in_place_index<I - 4>);
    }
  }

  template <std::size_t I>
  constexpr auto& get(std::in_place_index_t<I>) & noexcept {
    if constexpr (I == 0) {
      return static_cast<T0&>(t0_);
    } else if constexpr (I == 1) {
      return static_cast<T1&>(t1_);
    } else if constexpr (I == 2) {
      return static_cast<T2&>(t2_);
    } else if constexpr (I == 3) {
      return static_cast<T3&>(t3_);
    } else {
      return rest_.get(std::in_place_index<I - 4>);
    }
  }

  template <std::size_t I>
  constexpr auto&& get(std::in_place_index_t<I>) && noexcept {
    if constexpr (I == 0) {
      return static_cast<T0&&>(t0_);
    } else if constexpr (I == 1) {
      return static_cast<T1&&>(t1_);
    } else if constexpr (I == 2) {
      return static_cast<T2&&>(t2_);
    } else if constexpr (I == 3) {
      return static_cast<T3&&>(t3_);
    } else {
      return static_cast<tail_type&&>(rest_).get(std::in_place_index<I - 4>);
    }
  }

  template <std::size_t I>
  constexpr auto&& get(std::in_place_index_t<I>) const&& noexcept {
    if constexpr (I == 0) {
      return static_cast<const T0&&>(t0_);
    } else if constexpr (I == 1) {
      return static_cast<const T1&&>(t1_);
    } else if constexpr (I == 2) {
      return static_cast<const T2&&>(t2_);
    } else if constexpr (I == 3) {
      return static_cast<const T3&&>(t3_);
    } else {
      return static_cast<const tail_type&&>(rest_).get(
          std::in_place_index<I - 4>);
    }
  }
};

class union_storage_empty {};

template <typename... Ts> class union_storage_zero_size {
  static_assert((std::is_empty_v<Ts> && ...), "All Ts must be zero size");
  static_assert((std::is_trivially_default_constructible_v<Ts> && ...),
                "All Ts must be trivially default constructible");
  static_assert((std::is_trivially_destructible_v<Ts> && ...),
                "All Ts must be trivially destructible");

private:
  [[no_unique_address]] std::tuple<Ts...> impl_;

public:
  union_storage_zero_size() = default;

  template <std::size_t I, typename... Args>
  explicit constexpr union_storage_zero_size(std::in_place_index_t<I>,
                                             Args&&... args) {
    // not getting stored due to being zero sized
    std::tuple_element_t<I, std::tuple<Ts...>>(static_cast<Args&&>(args)...);
  }

  template <typename T, typename... Args>
  explicit constexpr union_storage_zero_size(std::in_place_type_t<T>,
                                             Args&&... args) {
    // TODO: check tuple contains element
    T(static_cast<Args&&>(args)...);
  }

  constexpr ~union_storage_zero_size() noexcept
    requires(!(std::is_trivially_destructible_v<Ts> && ...))
  {}

  ~union_storage_zero_size()
    requires((std::is_trivially_destructible_v<Ts> && ...))
  = default;

  template <std::size_t I>
  constexpr auto& get(std::in_place_index_t<I>) & noexcept {
    return std::get<I>(impl_);
  }

  template <std::size_t I>
  constexpr const auto& get(std::in_place_index_t<I>) const& noexcept {
    return std::get<I>(impl_);
  }

  template <std::size_t I>
  constexpr auto&& get(std::in_place_index_t<I>) && noexcept {
    return std::get<I>(std::move(impl_));
  }

  template <std::size_t I>
  constexpr const auto&& get(std::in_place_index_t<I>) const&& noexcept {
    return std::get<I>(std::move(impl_));
  }

  template <std::size_t I, typename... Args>
  constexpr void emplace(std::in_place_index_t<I> i, Args&&... args) {
    std::construct_at(std::addressof(get<I>(i)), static_cast<Args&&>(args)...);
  }

  template <std::size_t I>
  constexpr void destroy(std::in_place_index_t<I> i) noexcept {
    std::destroy_at(std::addressof(get<I>(i)));
  }
};
} // namespace detail

template <typename... Ts> class union_storage {
  static_assert((!std::is_reference_v<Ts> && ...),
                "union_storage cannot hold references due to UB with "
                "assignment and comparison.");

private:
  using impl_type = std::conditional_t<
      sizeof...(Ts) == 0, detail::union_storage_empty,
      std::conditional_t<(std::is_empty_v<Ts> && ...) &&
                             (std::is_trivially_default_constructible_v<Ts> &&
                              ...) &&
                             (std::is_trivially_destructible_v<Ts> && ...),
                         detail::union_storage_zero_size<Ts...>,
                         detail::union_storage_impl<Ts...>>>;

private:
  [[no_unique_address]] impl_type impl_;

public:
  template <std::size_t I, typename... Args>
  explicit constexpr union_storage(std::in_place_index_t<I> i, Args&&... args)
      : impl_(i, static_cast<Args&&>(args)...) {}

  ~union_storage() = default;

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr auto& get(std::in_place_index_t<I> i) & noexcept {
    return impl_.get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr const auto& get(std::in_place_index_t<I> i) const& noexcept {
    return impl_.get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr auto&& get(std::in_place_index_t<I> i) && noexcept {
    return std::move(impl_).get(i);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr const auto&& get(std::in_place_index_t<I> i) const&& noexcept {
    return std::move(impl_).get(i);
  }

  template <std::size_t I, typename... Args>
    requires(I < sizeof...(Ts))
  constexpr void emplace(std::in_place_index_t<I> i, Args&&... args) {
    impl_.emplace(i, static_cast<Args&&>(args)...);
  }

  template <std::size_t I>
    requires(I < sizeof...(Ts))
  constexpr void destroy(std::in_place_index_t<I> i) noexcept {
    impl_.destroy(i);
  }
};
} // namespace ely
