#pragma once

#include <cassert>
#include <iterator>
#include <memory>
#include <span>
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

struct align_result {
  void* p;
  std::size_t space;

  constexpr operator bool() const noexcept { return p != nullptr; }
};

class bump_block {
public:
  static constexpr std::size_t block_capacity = 32 * 1024 * 1024;

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

  align_result can_align_to(std::size_t sz, std::size_t align) const {
    void* data_p = std::bit_cast<void*>(data());
    std::size_t space = remaining_bytes();
    void* p = std::align(align, sz, data_p, space);
    return {.p = p, .space = space};
  }

  bool align_to(std::size_t sz, std::size_t align) {
    align_result ar = can_align_to(sz, align);
    if (ar) {
      space_ = ar.space;
      return true;
    }

    return false;
  }

  [[nodiscard]] void* allocate(std::size_t sz, std::size_t align) {
    bool align_success = align_to(sz, align);
    assert(align_success &&
           "Should've ensured there's enough space in this block");
    return std::bit_cast<void*>(data() + size_bytes());
  }

  bool can_allocate(std::size_t sz, std::size_t align) const {
    return can_align_to(sz, align);
  }

  // returns first out
  template <typename T, typename F>
  std::span<T> copy(T* first, T* last, F new_block) {
    std::size_t n = last - first;
    std::size_t n_bytes = sizeof(T) * n;

    if (can_allocate(n_bytes, alignof(T))) {
      T* start = (T*)allocate(n_bytes, alignof(T));
      T* end = std::copy(first, last, start);
      return {start, end};
    } else {
      auto* block = new_block(this, n_bytes);
      return block->copy(first, last, new_block);
    }
  }

  template <typename It, typename F>
  void try_copy(It first, It last, F new_block) {
    using it_traits = std::iterator_traits<It>;
    using value_t = typename it_traits::value_type;

    void* p = align_to(sizeof(value_t), alignof(value_t));
    if (!p) {
      impl::bump_block* block = new_block(this);
      block->try_copy(first, last, new_block);
      return;
    }

    std::size_t remaining = remaining_bytes() / sizeof(value_t);
    value_t* pstart = static_cast<value_t>(p);
    value_t* pcurr = pstart;
    for (std::size_t i = 0; i != remaining; ++i) {
      std::construct_at(pcurr++, *first++);
      if (first == last) {
        impl::bump_block* block = new_block(this);
        block->try_copy(pstart, pcurr, new_block);
        block->try_copy(first, last, new_block);
        return;
      }
    }
  }

  static bump_block* create(bump_block* prev, std::size_t min_cap) {
    auto this_block_capacity = std::max(min_cap, block_capacity);

    void* block_alloc =
        ::operator new(sizeof(bump_block) + this_block_capacity);
    return std::construct_at((impl::bump_block*)block_alloc, prev,
                             this_block_capacity);
  }
};
} // namespace impl

class bump {
public:
  template <typename T> using ptr_type = bump_ptr<T>;

private:
  struct new_block_fn {
    bump* self;

    impl::bump_block* operator()(impl::bump_block* b, std::size_t capacity) {
      self->current_ = impl::bump_block::create(b, capacity);
      return self->current_;
    }
  };

private:
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

  template <typename T> std::span<T> copy(T* first, T* last) {
    return current_->copy(first, last, new_block_fn{this});
  }

private:
  void check_current(std::size_t sz, std::size_t align) {
    if (!current_ || !current_->can_allocate(sz, align)) {
      // need to allocate new block
      current_ = impl::bump_block::create(current_, sz);
    }
  }

  void* allocate(std::size_t sz, std::size_t align) {
    check_current(sz, align);
    return current_->allocate(sz, align);
  }
};
} // namespace arena
} // namespace ely