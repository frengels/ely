#pragma once

#include <cassert>
#include <string>

namespace mli {
/// @brief
/// @tparam Char
/// @tparam Traits
/// string view which allows itself to be constructed from 2 iterators
template <typename Char, typename Traits = std::char_traits<Char>>
class basic_string_view {
public:
  using traits_type = Traits;
  using value_type = Char;
  using pointer = value_type *;
  using const_pointer = const value_type *;
  using reference = value_type &;
  using const_reference = const value_type &;
  using const_iterator = const_pointer;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using iterator = const_iterator;
  using reverse_iterator = const_reverse_iterator;
  using size_type = std::size_t;
  using difference_type = std::make_signed_t<size_type>;

private:
  const_pointer str_;
  size_type len_;

public:
  basic_string_view() = default;

  constexpr basic_string_view(const_pointer str)
      : basic_string_view(str, traits_type::length(str)) {}
  constexpr basic_string_view(const_pointer str, size_type len)
      : str_(str), len_(len) {}
  constexpr basic_string_view(const_iterator begin, const_iterator end)
      : basic_string_view(std::to_address(begin),
                          static_cast<size_type>(std::distance(begin, end))) {}

  constexpr size_type size() const { return len_; }
  constexpr size_type length() const { return size(); }
  constexpr const_pointer data() const { return str_; }
  constexpr bool empty() const { return size() == size_type{}; }

  constexpr const_reference operator[](size_type offset) const {
    assert(offset < size());
    return str_[offset];
  }

  constexpr const_iterator begin() const { return str_; }
  constexpr const_iterator cbegin() const { return begin(); }
  constexpr const_iterator end() const { return str_ + len_; }
  constexpr const_iterator cend() const { return end(); }

  constexpr const_reverse_iterator rbegin() const {
    const_reverse_iterator{end()};
  }
  constexpr const_reverse_iterator rend() const {
    const_reverse_iterator{begin()};
  }
  constexpr const_reverse_iterator crbegin() const { return rbegin(); }
  constexpr const_reverse_iterator crend() const { return rend(); }

  constexpr void swap(basic_string_view &other) noexcept {
    using std::swap;
    swap(str_, other.str_);
    swap(len_, other.len_);
  }
};

using string_view = basic_string_view<char>;
} // namespace mli