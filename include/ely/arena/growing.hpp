#pragma once

#include "ely/arena/basic.hpp"

namespace ely {
namespace arena {
namespace detail {
struct growing_fn {
  std::size_t current_capacity;

  constexpr growing_fn(std::size_t initial_capacity = 1024)
      : current_capacity(initial_capacity) {}

  constexpr std::size_t operator()(std::size_t sz) {
    auto next_capacity = std::max(sz, 2 * current_capacity);
    current_capacity = next_capacity;
    return next_capacity;
  }
};
} // namespace detail

struct growing : basic_arena<detail::growing_fn> {
  using base = basic_arena<detail::growing_fn>;

  using base::base;

  constexpr growing(std::size_t initial_capacity = 1024)
      : base(detail::growing_fn(initial_capacity)) {}
};
} // namespace arena
} // namespace ely