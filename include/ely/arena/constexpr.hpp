#pragma once

#include <vector>

#include <cassert>

namespace ely {
namespace arena {
namespace detail {
class destructible_ptr {
  void* ptr_;
  std::size_t size_; // not necessary, but doesn't hurt to track
  void (*deleter_)(void*, std::size_t) noexcept;

public:
  template <typename T>
  constexpr destructible_ptr(T* ptr, std::size_t size)
      : ptr_(ptr), size_(size), deleter_([](void* p, std::size_t s) noexcept {
          static_assert(std::is_nothrow_destructible_v<T>,
                        "T must be nothrow destructible");
          delete[] static_cast<T*>(p);
        }) {}

  constexpr destructible_ptr(destructible_ptr&& other) noexcept
      : ptr_(other.ptr_), size_(other.size_), deleter_(other.deleter_) {
    other.ptr_ = nullptr;
    other.size_ = 0;
    other.deleter_ = nullptr;
  }

  constexpr destructible_ptr& operator=(destructible_ptr&& other) noexcept {
    clear();
    using std::swap;
    swap(ptr_, other.ptr_);
    swap(size_, other.size_);
    swap(deleter_, other.deleter_);
    return *this;
  }

  destructible_ptr(const destructible_ptr&) = delete;
  destructible_ptr& operator=(const destructible_ptr&) = delete;

  constexpr ~destructible_ptr() noexcept { clear(); }

  constexpr void clear() noexcept {
    if (ptr_)
      deleter_(ptr_, size_);
    ptr_ = nullptr;
    size_ = 0;
    deleter_ = nullptr;
  }
};
} // namespace detail

// arena to be used in constexpr contexts, doesn't do anything remotely
// performant as this is at compile time.
// also this requires c++26 for constexpr casting to and from void*.
class constexpr_ {
private:
  std::vector<detail::destructible_ptr> allocations_;

public:
  constexpr_() = default;

  constexpr std::byte* allocate_bytes(
      std::size_t size,
      [[maybe_unused]] std::size_t alignment = alignof(std::max_align_t)) {
    check_consteval();
    std::byte* ptr = new std::byte[size];
    allocations_.emplace_back(ptr, size);
    return ptr;
  }

  template <typename T> constexpr T* allocate(std::size_t n = 1) {
    check_consteval();
    auto* ptr = new T[n];
    allocations_.emplace_back(ptr, n);
    return ptr;
  }

  constexpr void clear() noexcept {
    check_consteval();
    allocations_.clear();
  }

private:
  constexpr void check_consteval() const {
    assert(std::is_constant_evaluated() &&
           "constexpr_ arena should only be used in "
           "constant evaluated contexts");
  }
};
} // namespace arena
} // namespace ely