#pragma once

#include "ely/config.h"
#include <memory>
#include <new>
#include <utility>

#include <cassert>

namespace ely {
namespace arena {
namespace detail {
struct block {
  block* prev;
  std::size_t capacity;
  // offset 16 bytes, correct for max_align_t
  alignas(alignof(std::max_align_t)) std::byte data[];
};
} // namespace detail

template <typename GrowthFn> class basic_arena {
private:
  std::byte* cur_ptr_;
  std::byte* end_ptr_;
  detail::block* current_block_;
  [[no_unique_address]] GrowthFn growth_fn_;

public:
  basic_arena() = default;
  basic_arena(GrowthFn growth_fn) : growth_fn_(std::move(growth_fn)) {}

  ~basic_arena() {
    while (current_block_) {
      detail::block* prev = current_block_->prev;
      ::operator delete(current_block_,
                        sizeof(detail::block) + current_block_->capacity);
      current_block_ = prev;
    }
  }

  std::byte* allocate_bytes(std::size_t size,
                            std::size_t alignment = alignof(std::max_align_t)) {
    void* ptr = reinterpret_cast<void*>(cur_ptr_);
    std::size_t space = end_ptr_ - cur_ptr_;
    std::byte* aligned_ptr =
        reinterpret_cast<std::byte*>(std::align(alignment, size, ptr, space));
    if (!aligned_ptr) [[unlikely]] {
      allocate_block(size, alignment);
      aligned_ptr = cur_ptr_; // already aligned in allocate_block
    }

    cur_ptr_ = aligned_ptr + size;
    return aligned_ptr;
  }

  template <typename T> T* allocate(std::size_t count = 1) {
    return reinterpret_cast<T*>(allocate_bytes(sizeof(T) * count, alignof(T)));
  }

private:
  ELY_NOINLINE ELY_COLD void allocate_block(std::size_t size,
                                            std::size_t alignment) {
    std::size_t block_size = growth_fn_(size);
    assert(block_size > size && "growth function must return a size larger "
                                "than the requested allocation");
    detail::block* new_block = static_cast<detail::block*>(
        ::operator new(sizeof(detail::block) + block_size + alignment));
    new_block->prev = current_block_;
    new_block->capacity = block_size + alignment;
    current_block_ = new_block;
    cur_ptr_ = new_block->data;
    end_ptr_ = cur_ptr_ + block_size;
    std::size_t space = end_ptr_ - cur_ptr_;
    void* p = reinterpret_cast<void*>(cur_ptr_);
    std::byte* aligned_ptr =
        reinterpret_cast<std::byte*>(std::align(alignment, size, p, space));
    assert(aligned_ptr && "block size must be large enough to fit the "
                          "requested allocation including alignment padding");
    cur_ptr_ = aligned_ptr;
  }
};
} // namespace arena
} // namespace ely
