#pragma once

#include <array>
#include <cstddef>
#include <type_traits>

#include "ely/util/maybe_uninit.hpp"

namespace ely {
namespace arena {
template <typename T, std::size_t N> class stack {
  static_assert(std::is_trivially_default_constructible_v<T>,
                "TODO: make this work with non-trivial types, having some "
                "trouble in constexpr context");

private:
  // std::array<maybe_uninit<T>, N> storage_{};
  std::array<T, N> storage_{};
  std::size_t offset_{};

public:
  stack() = default;

  constexpr ~stack() { clear(); }

  constexpr T* allocate(std::size_t count = 1) noexcept {
    if (offset_ + count > N) [[unlikely]] {
      return nullptr;
    }

    // ely::maybe_uninit<T>* uninit_ptr = storage_.data() + offset_;
    // for (auto it = uninit_ptr; it != uninit_ptr + count; ++it) {
    //   it->assume_init();
    // }
    T* ptr = storage_.data() + offset_;

    offset_ += count;
    // return uninit_ptr->get_ptr();
    return ptr;
  }

  constexpr void clear() {
    // objects need to be destroyed in consteval context
    // if consteval {
    //   for (std::size_t i = 0; i != offset_; ++i) {
    //     storage_[i].destroy();
    //   }
    // }
    offset_ = 0;
  }
};
} // namespace arena
} // namespace ely