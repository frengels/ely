#pragma once

#include <concepts>
#include <memory>

namespace ely {
namespace arena {
namespace detail {
template <typename T>
concept cv_unqualified = std::same_as<T, std::remove_cvref_t<T>>;

template <typename Alloc, typename T>
concept allocator_fns = requires(Alloc a, std::size_t n) {
  typename Alloc::pointer;

  { a.allocate(n) } -> std::same_as<typename Alloc::pointer>;
};

template <typename Alloc, typename... Args>
concept has_construct =
    requires(Alloc a, typename Alloc::pointer p, Args&&... args) {
      { a.construct(p, static_cast<Args &&>(args)...) };
    };
} // namespace detail

template <typename Alloc, typename T>
concept allocator =
    detail::cv_unqualified<T> && std::same_as<typename Alloc::value_type, T> &&
    detail::allocator_fns<Alloc, T>;

template <typename Arena> class ref_allocator {
public:
  using value_type = typename Arena::value_type;
  using pointer = typename Arena::pointer;

private:
  Arena* arena_;

public:
  constexpr ref_allocator(Arena& a) : arena_(std::addressof(a)) {}

  constexpr pointer allocate(std::size_t n) { return arena_->allocate(n); }

  template <typename... Args>
  constexpr void construct(pointer p, Args&&... args) {
    if constexpr (detail::has_construct<Arena, Args...>) {
      arena_->construct(p, static_cast<Args&&>(args)...);
    } else {
      std::construct_at(p, static_cast<Args&&>(args)...);
    }
  }
};

} // namespace arena
} // namespace ely