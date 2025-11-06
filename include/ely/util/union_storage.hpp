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
      return t0_;
    } else if constexpr (I == 1) {
      return t1_;
    } else if constexpr (I == 2) {
      return t2_;
    } else if constexpr (I == 3) {
      return t3_;
    } else {
      return rest_.get(std::in_place_index<I - 4>);
    }
  }

  template <std::size_t I>
  constexpr auto& get(std::in_place_index_t<I>) & noexcept {
    if constexpr (I == 0) {
      return t0_;
    } else if constexpr (I == 1) {
      return t1_;
    } else if constexpr (I == 2) {
      return t2_;
    } else if constexpr (I == 3) {
      return t3_;
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
} // namespace detail

template <typename... Ts>
using union_storage = detail::union_storage_impl<Ts...>;

namespace detail {
// for use in a variant with a single element
template <std::size_t I> class constant_index {
public:
  constant_index() = default;

  constexpr std::size_t operator()() const { return I; }
};

template <std::uint64_t I> constexpr auto get_index_type_impl() {
  if constexpr (I < std::numeric_limits<std::uint8_t>::max()) {
    return std::uint8_t{};
  } else if constexpr (I < std::numeric_limits<std::uint16_t>::max()) {
    return std::uint16_t{};
  } else if constexpr (I < std::numeric_limits<std::uint32_t>::max()) {
    return std::uint32_t{};
  } else {
    return std::uint64_t{};
  }
}

template <typename I, I... Is>
constexpr std::array<I, sizeof...(Is)>
integer_sequence_to_array(std::integer_sequence<I, Is...>) {
  return std::array<I, sizeof...(Is)>{Is...};
}

template <typename T, std::size_t N>
constexpr bool contains(const std::array<T, N>& arr, const T& val) {
  std::find(arr.begin(), arr.end(), val) != arr.end();
}

template <std::size_t N>
constexpr std::size_t rmap(const std::array<std::size_t, N>& arr,
                           std::size_t idx) {
  auto it = std::find(arr.begin(), arr.end(), idx);
  assert(it != arr.end());
  return std::distance(arr.begin(), it);
}
} // namespace detail

template <std::uint64_t I>
using get_index_type_t = decltype(detail::get_index_type_impl<I>());

namespace detail {
template <typename BaseT, typename... Ts> class variant_interface {
public:
  variant_interface() = default;

private:
  constexpr auto& base() & noexcept { return static_cast<BaseT&>(*this); }

  constexpr const auto& base() const& noexcept {
    return static_cast<const BaseT&>(*this);
  }

  constexpr auto&& base() && noexcept { return static_cast<BaseT&&>(*this); }

  constexpr const auto&& base() const&& noexcept {
    return static_cast<const BaseT&&>(*this);
  }
};

template <bool B, typename... Ts> class variant_base_impl;

template <typename... Ts>
using variant_base =
    variant_base_impl<(std::is_trivially_destructible_v<Ts> && ...), Ts...>;

template <std::size_t I, typename T> struct vmap;

template <std::size_t... Is, typename... Ts>
class variant_base_impl<true, vmap<Is, Ts>...> {
private:
  inline static constexpr std::array<std::size_t, sizeof...(Is)> index_mapping =
      integer_sequence_to_array(std::integer_sequence<std::size_t, Is...>{});

  template <std::size_t I>
  inline static constexpr bool holds_index = detail::contains(index_mapping, I);

public:
  using index_type = get_index_type_t<std::max(Is...)>;

private:
  [[no_unique_address]] union_storage<Ts...> store_;
  [[no_unique_address]] index_type idx_;

public:
  constexpr variant_base_impl()
      : store_(std::in_place_index<0>), idx_(index_mapping[0]) {}

  template <std::size_t I, typename... Args>
    requires(holds_index<I>)
  constexpr explicit variant_base_impl(std::in_place_index_t<I>, Args&&... args)
      : store_(std::in_place_index<rmap(index_mapping, I)>,
               static_cast<Args&&>(args)...) {}

  constexpr std::size_t index() const noexcept { return idx_; }

  template <std::size_t I>
    requires(holds_index<I>)
  constexpr auto& get_unchecked(std::in_place_index_t<I>) & noexcept {
    return store_.get(std::in_place_index<rmap(index_mapping, I)>);
  }

  template <std::size_t I>
    requires(holds_index<I>)
  constexpr const auto&
  get_unchecked(std::in_place_index_t<I>) const& noexcept {
    return store_.get(std::in_place_index<rmap(index_mapping, I)>);
  }

  template <std::size_t I>
    requires(holds_index<I>)
  constexpr auto&& get_unchecked(std::in_place_index_t<I>) && noexcept {
    return std::move(store_).get(std::in_place_index<rmap(index_mapping, I)>);
  }

  template <std::size_t I>
    requires(holds_index<I>)
  constexpr const auto&&
  get_unchecked(std::in_place_index_t<I>) const&& noexcept {
    return std::move(store_).get(std::in_place_index<rmap(index_mapping, I)>);
  }

  template <std::size_t I, typename... Args>
    requires(holds_index<I>)
  constexpr void emplace_unchecked(Args&&... args) {
    store_.emplace(std::in_place_index<rmap(index_mapping, I)>,
                   static_cast<Args&&>(args)...);
    idx_ = I;
  }
};

} // namespace detail
} // namespace ely
