#pragma once

#include <limits>
#include <memory>
#include <type_traits>
#include <utility>

namespace ely {

namespace detail {
template <bool B, typename... Ts> union union_storage_impl;
}

// template <typename... Ts> union union_storage {};

template <typename... Ts>
using union_storage =
    detail::union_storage_impl<(std::is_trivially_destructible_v<Ts> && ...),
                               Ts...>;

namespace detail {
template <bool B> union union_storage_impl<B> {};

template <typename T, typename... Ts>
union union_storage_impl<false, T, Ts...> {
  T first_;
  union_storage<Ts...> rest_;

  template <typename... Args>
  constexpr union_storage_impl(std::in_place_index_t<0>, Args&&... args)
      : first_(static_cast<Args&&>(args)...) {}

  template <std::size_t I, typename... Args>
  constexpr union_storage_impl(std::in_place_index_t<I>, Args&&... args)
      : rest_(std::in_place_index<I - 1>, static_cast<Args&&>(args)...) {}

  constexpr ~union_storage_impl() noexcept {}

  template <typename... Args>
  constexpr void emplace(std::in_place_index_t<0>, Args&&... args) {
    std::construct_at(&first_, static_cast<Args&&>(args)...);
  }

  template <std::size_t I, typename... Args>
  constexpr void emplace(std::in_place_index_t<I>, Args&&... args) {
    return rest_.emplace(std::in_place_index<I - 1>,
                         static_cast<Args&&>(args)...);
  }

  constexpr void destroy(std::in_place_index_t<0>) noexcept {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      std::destroy_at(&first_);
    }
  }

  template <std::size_t I>
  constexpr void destroy(std::in_place_index_t<I>) noexcept {
    return rest_.destroy(std::in_place_index<I - 1>);
  }

  constexpr T& get(std::in_place_index_t<0>) & noexcept { return first_; }
  constexpr const T& get(std::in_place_index_t<0>) const& noexcept {
    return first_;
  }
  constexpr T&& get(std::in_place_index_t<0>) && noexcept {
    return static_cast<T&&>(first_);
  }
  constexpr const T&& get(std::in_place_index_t<0>) const&& noexcept {
    return static_cast<const T&&>(first_);
  }

  template <std::size_t I>
  constexpr auto& get(std::in_place_index_t<I>) & noexcept {
    return rest_.get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr const auto& get(std::in_place_index_t<I>) const& noexcept {
    return rest_.get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr auto&& get(std::in_place_index_t<I>) && noexcept {
    return std::move(rest_).get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr const auto&& get(std::in_place_index_t<I>) const&& noexcept {
    return std::move(rest_).get(std::in_place_index<I - 1>);
  }
};

template <typename T, typename... Ts> union union_storage_impl<true, T, Ts...> {
  T first_;
  union_storage<Ts...> rest_;

  template <typename... Args>
  constexpr union_storage_impl(std::in_place_index_t<0>, Args&&... args)
      : first_(static_cast<Args&&>(args)...) {}

  template <std::size_t I, typename... Args>
  constexpr union_storage_impl(std::in_place_index_t<I>, Args&&... args)
      : rest_(std::in_place_index<I - 1>, static_cast<Args&&>(args)...) {}

  ~union_storage_impl() = default;

  template <typename... Args>
  constexpr void emplace_impl(std::false_type, std::in_place_index_t<0>,
                              Args&&... args) {
    std::construct_at(&first_, static_cast<Args&&>(args)...);
  }

  template <std::size_t I, typename... Args>
  constexpr void emplace_impl(std::false_type, std::in_place_index_t<I>,
                              Args&&... args) {
    rest_.emplace(std::in_place_index<I - 1>, static_cast<Args&&>(args)...);
  }

  template <std::size_t I, typename... Args>
  constexpr void emplace_impl(std::true_type, std::in_place_index_t<I>,
                              Args&&... args) {
    *this = union_storage_impl(std::in_place_index<I>,
                               static_cast<Args&&>(args)...);
  }

  template <std::size_t I, typename... Args>
  constexpr void emplace(std::in_place_index_t<I>, Args&&... args) {
    return emplace_impl(
        std::conjunction<std::is_trivially_move_assignable<T>,
                         std::is_trivially_move_assignable<Ts>...>{},
        std::in_place_index<I>, static_cast<Args&&>(args)...);
  }

  constexpr void destroy(std::in_place_index_t<0>) noexcept {
    if constexpr (!std::is_trivially_destructible_v<T>) {
      std::destroy_at(&first_);
    }
  }

  template <std::size_t I>
  constexpr void destroy(std::in_place_index_t<I>) noexcept {
    return rest_.destroy(std::in_place_index<I - 1>);
  }

  constexpr T& get(std::in_place_index_t<0>) & noexcept { return first_; }
  constexpr const T& get(std::in_place_index_t<0>) const& noexcept {
    return first_;
  }
  constexpr T&& get(std::in_place_index_t<0>) && noexcept {
    return static_cast<T&&>(first_);
  }
  constexpr const T&& get(std::in_place_index_t<0>) const&& noexcept {
    return static_cast<const T&&>(first_);
  }

  template <std::size_t I>
  constexpr auto& get(std::in_place_index_t<I>) & noexcept {
    return rest_.get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr const auto& get(std::in_place_index_t<I>) const& noexcept {
    return rest_.get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr auto&& get(std::in_place_index_t<I>) && noexcept {
    return std::move(rest_).get(std::in_place_index<I - 1>);
  }

  template <std::size_t I>
  constexpr const auto&& get(std::in_place_index_t<I>) const&& noexcept {
    return std::move(rest_).get(std::in_place_index<I - 1>);
  }
};

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
