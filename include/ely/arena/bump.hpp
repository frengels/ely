#pragma once

#include <cassert>
#include <memory>
#include <utility>

namespace ely {
namespace arena {
template <typename T> class bump_ptr {

private:
  T* p_;

public:
  explicit constexpr bump_ptr(T* p) : p_(p) {}

  constexpr T* get() const noexcept { return p_; }
  constexpr T* operator->() const noexcept { return get(); }
  constexpr T& operator*() const noexcept { return *get(); }

  constexpr operator bool() const noexcept { return p_; }
};

namespace impl {
template <typename Alloc, typename T>
inline constexpr bool allocates_ordinary_pointers =
    std::is_same_v<typename std::allocator_traits<Alloc>::pointer, T*>;

class bump_block {
private:
  bump_block* prev_;
  std::size_t space_;
  std::size_t capacity_;
  std::byte data_[];

public:
  bump_block(bump_block* prev, std::size_t capacity)
      : prev_(prev), space_(capacity), capacity_(capacity) {}

  constexpr std::byte* data() { return data_; }
  constexpr const std::byte* data() const { return data_; }
  constexpr std::size_t size_bytes() const {
    return capacity_bytes() - remaining_bytes();
  }
  constexpr std::size_t capacity_bytes() const { return capacity_; }
  constexpr std::size_t remaining_bytes() const { return space_; }

  constexpr bump_block* prev() const { return prev_; }

  bool can_allocate(std::size_t sz, std::size_t align) const {
    if (remaining_bytes() < sz)
      return false;

    void* data_p = const_cast<void*>(static_cast<const void*>(data()));
    std::size_t space = remaining_bytes();
    return std::align(align, sz, data_p, space);
  }

  [[nodiscard]] void* allocate(std::size_t sz, std::size_t align) {
    assert(can_allocate(sz, align));
    void* data_v = static_cast<void*>(data());
    size_t space = remaining_bytes();
    if (void* res =
            std::align(static_cast<std::size_t>(align), sz, data_v, space);
        res) {
      space -= sz;
      space_ = space;
      return res;
    }

    return nullptr;
  }
};
} // namespace impl

template <typename Alloc = std::allocator<std::byte>> class bump {
public:
  template <typename T> using ptr_type = bump_ptr<T>;

  using allocator_type =
      typename std::allocator_traits<Alloc>::template rebind_alloc<std::byte>;
  using alloc_traits = std::allocator_traits<allocator_type>;

  static_assert(impl::allocates_ordinary_pointers<allocator_type, std::byte>,
                "ely::arena::bump<Alloc> cannot handle allocators which "
                "don't return ordinary pointers.");

private:
  static constexpr std::size_t block_capacity = 32 * 1024 * 1024;

  [[no_unique_address]] allocator_type alloc_;
  impl::bump_block* current_;

public:
  bump() = default;

  ~bump() {
    while (current_) {
      impl::bump_block* work = std::exchange(current_, current_->prev());
      std::size_t capacity = work->capacity_bytes();
      std::destroy_at(work);
      alloc_traits::deallocate(alloc_, (std::byte*)work, capacity);
    }
  }

  template <typename T, typename... Args>
  constexpr bump_ptr<T> make(Args&&... args) {
    static_assert(std::is_trivially_destructible_v<T>,
                  "ely::arena::bump doesn't destroy objects, this is unsafe.");
    using alloc_t = typename alloc_traits::template rebind_alloc<T>;
    using alloc_t_traits = std::allocator_traits<alloc_t>;

    T* p = static_cast<T*>(allocate(sizeof(T), alignof(T)));
    alloc_t alloc{alloc_};
    alloc_t_traits::construct(alloc, p, static_cast<Args&&>(args)...);
    return bump_ptr<T>{p};
  }

private:
  void check_current(std::size_t sz, std::size_t align) {
    if (!current_ || !current_->can_allocate(sz, align)) {
      // need to allocate new block
      auto* old_current = current_;
      std::size_t this_block_capacity = std::max(sz, block_capacity);
      std::byte* allocation = alloc_traits::allocate(
          alloc_, sizeof(impl::bump_block) + this_block_capacity);

      current_ = std::construct_at((impl::bump_block*)allocation, current_,
                                   this_block_capacity);
    }
  }

  void* allocate(std::size_t sz, std::size_t align) {
    check_current(sz, align);
    return current_->allocate(sz, align);
  }
};
} // namespace arena
} // namespace ely