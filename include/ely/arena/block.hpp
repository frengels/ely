#pragma once

#include <array>
#include <cassert>
#include <forward_list>
#include <memory>

namespace ely {
namespace mem {
// block used for arena allocation
template <typename T, std::size_t Cap> class fixed_block {
  // TODO: reenable this check
  //   static_assert(std::is_trivially_destructible_v<T>,
  //                 "T must be trivially destructible because values will never
  //                 be " "destroyed.");

public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;

  using iterator = pointer;
  using const_iterator = const_pointer;

private:
  pointer current_;
  std::array<T, Cap> data_;

public:
  explicit constexpr fixed_block() : current_(data_.data()) {}

  static constexpr std::size_t capacity() { return Cap; }
  constexpr std::size_t size() const { return end() - begin(); }
  constexpr std::size_t remaining() const { return capacity() - size(); }

  constexpr pointer data() { return data_.data(); }
  constexpr const_pointer data() const { return data_.data(); }

  constexpr iterator begin() { return data_.data(); }
  constexpr iterator end() { return current_; }

  constexpr const_iterator begin() const { return data_.data(); }
  constexpr const_iterator end() const { return current_; }

  constexpr pointer allocate(std::size_t n) {
    assert(remaining() >= n && "Not enough space to allocate");
    pointer res = current_;
    current_ += n;
    return res;
  }
};
} // namespace mem

namespace arena {
template <typename T, std::size_t Cap, typename Alloc = std::allocator<T>>
class fixed_block {
public:
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using reference = value_type&;
  using const_reference = const value_type&;

private:
  using alloc_t = Alloc;
  using alloc_traits_t = std::allocator_traits<alloc_t>;

  using block_t = mem::fixed_block<T, Cap>;

  using alloc_block = alloc_traits_t::template rebind_alloc<block_t>;

private:
  std::forward_list<block_t, alloc_block> blocks_;
  [[no_unique_address]] alloc_t alloc_;

public:
  fixed_block() = default;
  fixed_block(alloc_t alloc) : alloc_(alloc) {}

  static constexpr std::size_t capacity() { return Cap; }
  // the maximum allocation permitted, equal to provided Cap
  static constexpr std::size_t max_size() { return capacity(); }

  constexpr pointer allocate(std::size_t n) {
    if (n > max_size()) [[unlikely]]
      throw std::bad_alloc();
    if (!blocks_.empty() && blocks_.begin()->remaining() <= n) {
      return blocks_.begin()->allocate(n);
    }

    auto& block = blocks_.emplace_front();
    return block.allocate(n);
  }

  template <typename... Args>
  constexpr void construct(pointer p, Args&&... args) {
    alloc_traits_t::construct(alloc_, p, static_cast<Args&&>(args)...);
  }
};
} // namespace arena
} // namespace ely