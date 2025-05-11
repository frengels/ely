#pragma once

#include <memory>

namespace ely {
namespace arena {
// simply creates data without any deallocation concerns
template <typename T, typename Alloc = std::allocator<T>> class dumb_typed {
private:
  using alloc_traits = std::allocator_traits<Alloc>;

private:
  [[no_unique_address]] Alloc alloc_;

public:
  dumb_typed() = default;

  constexpr dumb_typed get_arena_allocator() const { return *this; }

  constexpr T* allocate(std::size_t n) {
    return alloc_traits::allocate(alloc_, n);
  }

  template <typename... Args> constexpr void construct(T* p, Args&&... args) {
    new ((void*)p) T(static_cast<Args&&>(args)...);
    // return alloc_traits::construct(alloc_, p, static_cast<Args&&>(args)...);
  }
};
} // namespace arena
} // namespace ely