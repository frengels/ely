#pragma once

#include <cassert>
#include <memory>
#include <utility>

namespace ely {
template <typename T, typename Alloc = std::allocator<T>> class alloc_buffer {
public:
  using value_type = T;
  using allocator_type = Alloc;
  using iterator = T*;
  using const_iterator = const T*;

private:
  using alloc_traits = std::allocator_traits<allocator_type>;

private:
  T* begin_;
  T* end_;
  [[no_unique_address]] allocator_type alloc_;

public:
  alloc_buffer() = default;

  explicit constexpr alloc_buffer(std::size_t n, allocator_type alloc = {})
      : begin_(alloc_traits::allocate(alloc_, n)), end_(begin_ + n),
        alloc_(alloc) {}

  constexpr alloc_buffer(alloc_buffer&& other)
      : begin_(std::exchange(other.begin_, nullptr)),
        end_(std::exchange(other.end_, nullptr)),
        alloc_(std::exchange(other.alloc_, allocator_type{})) {}

  // FIXME: we can move assign trivially destructible as there's nothing to
  // destroy it's impossible to know the number of constructed objects,
  // therefore we can't destroy them
  alloc_buffer& operator=(alloc_buffer&&) = delete;

  constexpr ~alloc_buffer() { clear(); }

  constexpr allocator_type get_allocator() const { return alloc_; }

  constexpr std::size_t capacity() const { return std::distance(begin_, end_); }

  constexpr T* begin() const { return begin_; }
  constexpr T* end() const { return end_; }

  // grow the allocation, active_size indicates the items which will need to be
  // moved over. Can also be used to shrink.
  // move_elements moves items from the old to new
  template <typename F>
  constexpr void change_capacity(std::size_t n, F move_elements) {
    // FIXME: what happens with this allocation when exception thrown
    T* new_alloc = alloc_traits::allocate(alloc_, n);
    // move active elements over
    move_elements(new_alloc);
    clear();
    begin_ = new_alloc;
    end_ = begin_ + n;
  }

  template <typename F> constexpr void grow(std::size_t n, F move_elements) {
    assert(n > capacity());
    change_capacity(n, std::move(move_elements));
  }

  constexpr void clear() noexcept {
    if (begin_) {
      alloc_traits::deallocate(alloc_, begin_, capacity());
      begin_ = nullptr;
      end_ = nullptr;
    }
  }

  friend constexpr void swap(alloc_buffer& lhs, alloc_buffer& rhs) noexcept(
      std::is_nothrow_swappable_v<allocator_type>) {
    using std::swap;
    swap(lhs.begin_, rhs.begin_);
    swap(lhs.end_, rhs.end_);
    swap(lhs.alloc_, rhs.alloc_);
  }
};
} // namespace ely