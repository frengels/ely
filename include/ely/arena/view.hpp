#pragma once

#include <utility>

namespace ely {
namespace arena {
namespace detail {
template <typename Arena, typename T>
concept untyped_arena_for = requires(Arena& arena, std::size_t n) {
  { arena.template allocate<T>(n) } -> std::same_as<T*>;
};

template <typename Arena, typename T>
concept typed_arena_for = requires(Arena& arena, std::size_t n) {
  { arena.allocate(n) } -> std::same_as<T*>;
};

template <typename Arena>
concept byte_arena =
    requires(Arena& arena, std::size_t n, std::size_t alignment) {
      { arena.allocate_bytes(n, alignment) } -> std::same_as<std::byte*>;
    };
} // namespace detail

template <typename Arena, typename T> class view_untyped {
  static_assert(detail::untyped_arena_for<Arena, T>,
                "Arena must support untyped allocation for T");

private:
  Arena* arena_;

public:
  constexpr view_untyped(Arena& arena) : arena_(std::addressof(arena)) {}

  constexpr auto allocate(std::size_t count = 1)
      -> decltype(arena_->template allocate<T>(count, alignof(T))) {
    return arena_->template allocate<T>(count, alignof(T));
  }
};

template <typename Arena, typename T> class view_typed {
  static_assert(detail::typed_arena_for<Arena, T>,
                "Arena must support typed allocation for T");

private:
  Arena* arena_;

public:
  constexpr view_typed(Arena& arena) : arena_(std::addressof(arena)) {}

  constexpr auto allocate(std::size_t count = 1)
      -> decltype(arena_->allocate(count)) {
    return arena_->allocate(count);
  }
};

template <typename Arena, typename T> class view_bytes {
  static_assert(detail::byte_arena<Arena>,
                "Arena must support byte allocation");

private:
  Arena* arena_;

public:
  constexpr view_bytes(Arena& arena) : arena_(std::addressof(arena)) {}

  constexpr auto allocate(std::size_t count = 1) -> T* {
    return reinterpret_cast<T*>(
        arena_->allocate_bytes(count * sizeof(T), alignof(T)));
  }
};

template <typename Arena, typename T> class view {
  static_assert(detail::untyped_arena_for<Arena, T> ||
                    detail::typed_arena_for<Arena, T>,
                "Arena must support either typed or untyped allocation for T");

private:
  std::conditional_t<detail::typed_arena_for<Arena, T>, view_typed<Arena, T>,
                     view_untyped<Arena, T>>
      impl_;

public:
  constexpr view(Arena& arena) : impl_(arena) {}

  constexpr auto allocate(std::size_t count = 1)
      -> decltype(impl_.allocate(count)) {
    return impl_.allocate(count);
  }
};
} // namespace arena
} // namespace ely