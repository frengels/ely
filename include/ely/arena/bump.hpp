#pragma once

#include <cassert>
#include <memory>
#include <type_traits>
#include <utility>

namespace ely {
namespace arena {
template <typename T> class bump_ptr {
  using element_type = std::remove_extent_t<T>;

private:
  element_type* p_;

public:
  explicit constexpr bump_ptr(element_type* p) : p_(p) {}

  constexpr element_type* get() const noexcept { return p_; }
  constexpr element_type* operator->() const noexcept { return get(); }
  constexpr element_type& operator*() const noexcept { return *get(); }

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

class bump {
public:
  template <typename T> using ptr_type = bump_ptr<T>;

private:
  static constexpr std::size_t block_capacity = 32 * 1024 * 1024;

  impl::bump_block* current_;

public:
  bump() = default;

  ~bump() {
    while (current_) {
      impl::bump_block* work = std::exchange(current_, current_->prev());
      std::size_t block_size =
          work->capacity_bytes() + sizeof(impl::bump_block);
      std::destroy_at(work);
      ::operator delete((void*)work, block_size);
    }
  }

  template <typename T, typename... Args>
  std::enable_if_t<!std::is_array_v<T>, bump_ptr<T>> make(Args&&... args) {
    static_assert(std::is_trivially_destructible_v<T>,
                  "ely::arena::bump doesn't destroy objects, this is unsafe.");

    T* p = static_cast<T*>(allocate(sizeof(T), alignof(T)));
    std::construct_at(p, static_cast<Args&&>(args)...);
    return bump_ptr<T>{p};
  }

  template <typename T>
  std::enable_if_t<std::is_unbounded_array_v<T>, bump_ptr<T>>
  make(std::size_t n) {
    using type_t = std::remove_extent_t<T>;
    static_assert(std::is_trivially_destructible_v<type_t>,
                  "ely::arena::bump doesn't destroy objects, this is unsafe.");
    type_t* p =
        static_cast<type_t*>(allocate(sizeof(type_t) * n, alignof(type_t)));

    if constexpr (!std::is_trivially_constructible_v<type_t>) {
      for (std::size_t i = 0; i != n; ++i)
        std::construct_at(p + i);
    }

    return bump_ptr<T>(p);
  }

  template <typename T>
  std::enable_if_t<std::is_bounded_array_v<T>, bump_ptr<T>> make() {
    using type_t = std::remove_extent_t<T>;
    static_assert(std::is_trivially_destructible_v<type_t>,
                  "ely::arena::bump doesn't destroy objects, this is unsafe.");
    return bump_ptr<T>(make<type_t[]>(std::extent_v<T>).get());
  }

private:
  void check_current(std::size_t sz, std::size_t align) {
    if (!current_ || !current_->can_allocate(sz, align)) {
      // need to allocate new block
      auto* old_current = current_;
      std::size_t this_block_capacity = std::max(sz, block_capacity);
      void* allocation =
          ::operator new(sizeof(impl::bump_block) + this_block_capacity);
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