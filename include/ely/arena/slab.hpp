#pragma once

#include "ely/arena/basic.hpp"
#include <cstddef>

namespace ely {
namespace arena {
namespace detail {
template <std::size_t SlabSize> struct slab_growth_fn {
  constexpr std::size_t operator()(std::size_t sz) const {
    return std::max(sz, SlabSize);
  }
};
} // namespace detail

template <std::size_t SlabSize>
struct slab : basic_arena<detail::slab_growth_fn<SlabSize>> {
  using base = basic_arena<detail::slab_growth_fn<SlabSize>>;

  using base::base;
};
} // namespace arena
} // namespace ely