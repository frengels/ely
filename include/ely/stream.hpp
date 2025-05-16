#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <ranges>
#include <span>
#include <string>

#include "ely/config.hpp"

namespace ely {
template <typename CharT, typename Traits = std::char_traits<CharT>>
// zero terminated string stream
class basic_zstring_stream {
public:
  using value_type = CharT;

  static constexpr value_type empty_value = '\0';

private:
  const value_type* str_;

public:
  explicit constexpr basic_zstring_stream(const CharT* str) : str_(str) {}

  constexpr const value_type* str() const { return str_; }
  constexpr const value_type* c_str() const { return str(); }

  constexpr const value_type& next() { return *str_++; }
  constexpr std::size_t next_n_into(value_type* out, std::size_t n) {
    auto it = str_;
    std::size_t i = 0;
    for (std::size_t i = 0; i != n; ++i) {
      if (str_[i] == empty_value)
        break;
      out[i] = str_[i];
    }
    str_ += i;
    return i;
  }
  constexpr const value_type& peek() const { return *str_; }
};

using zstring_stream = basic_zstring_stream<char>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_file_stream {
public:
  using char_type = CharT;
  using value_type = char_type;

  static constexpr value_type empty_value = '\0';

private:
  // opened file, we won't be closing this
  FILE* file_;

public:
  basic_file_stream(FILE* f) : file_(f) {}

  char_type next() {
    char out;
    return next_n_into(&out, 1);
    return out;
  }

  std::size_t next_n_into(char* out, std::size_t size) {
    return fill_buffer(out, size);
  }

private:
  std::size_t fill_buffer(char_type* buf, std::size_t len) {
    auto read = std::fread(buf, sizeof(char_type), len, file_);
    // add \0 if stream ended
    if (read < len) {
      *(buf + read) = '\0';
    }

    return read;
  }
};

using file_stream = basic_file_stream<char>;

template <typename Stream, typename Buffer> class buffered_stream {
public:
  using value_type =
      std::remove_cvref_t<decltype(std::declval<Stream&>().next())>;

  static_assert(
      std::is_trivial_v<value_type>,
      "This is not yet equiped for data more complicated than a char");

private:
  [[no_unique_address]] Stream stream_;
  Buffer buffer_;
  // determines the end of the active part of the buffer
  // if not at the buffer's end then stream_ is exhausted.
  typename Buffer::iterator end_;
  // the current value to be emitted
  // if equal to end_ and end_ is not the buffer end then this stream is
  // exhausted
  typename Buffer::iterator current_;
  // bool exhausted_{false};

public:
  buffered_stream() = default;

  constexpr buffered_stream(Stream stream, Buffer&& buf)
      : stream_(std::move(stream)), buffer_(std::move(buf)),
        end_(buffer_.end()), current_{buffer_.end()} {}

private:
  constexpr bool is_underlying_exhausted() const {
    return end_ != buffer_.end();
  }

  constexpr bool is_exhausted() const {
    return is_underlying_exhausted() && current_ == end_;
  }

  // the amount of active members in the buffer
  constexpr std::size_t remaining() const {
    return std::distance(current_, end_);
  }

public:
  constexpr std::size_t buffer_capacity() const { return buffer_.capacity(); }

  constexpr value_type& peek() {
    assert(*current_ != Stream::empty_value &&
           "Trying to peek an exhausted stream");
    if (current_ == buffer_.end())
      fill_buffer(buffer_);
    current_ = buffer_.begin();
    return *current_;
  }

  constexpr const value_type& next() {
    // check for exhaustion
    auto res = next_n(1);
    if (res.size() == 0)
      return Stream::empty_value;
    return res[0];
  }

  // returns the next n characters, afterwards gives access to internal buffer
  // for copying
  constexpr std::span<value_type> next_n(std::size_t len) {
    if (len > remaining() && is_underlying_exhausted()) {
      // nothing more to fetch
      // just return what we have
      auto res = std::span{current_, end_};
      current_ = end_;
      return res;
    }
    if (len > buffer_capacity()) {
      // requested length is longer than buffer, grow
      std::size_t moved = std::distance(current_, buffer_.end());
      std::size_t current_offset = std::distance(buffer_.begin(), current_);
      grow_buffer(len);
      std::size_t ret_len = std::min(
          len, static_cast<std::size_t>(std::distance(current_, end_)));
      auto res = std::span(current_, ret_len);
      current_ += ret_len;
      return res;
    }
    // check if data can be contiguous
    else if (current_ + len > buffer_.end()) {
      // doesn't fit, copy to start
      value_type* out_next =
          std::move(current_, buffer_.end(), buffer_.begin());
      // fill until the end
      value_type* end = fill_buffer(out_next, buffer_.end());
      end_ = end;
      current_ = buffer_.begin();
      auto res = std::span{buffer_.begin(), std::min(len, remaining())};
      current_ = buffer_.begin() + std::min(len, remaining());
      return res;
    } else {
      auto res = std::span{current_, len};
      current_ += len;
      return res;
    }
  }

  // continues gathering values until the predicate is satisfies, then returns
  // the range. Also returns if the stream runs out of elements.
  template <typename Pred>
  constexpr std::span<value_type> next_until(Pred pred) {
    for (auto it = current_; it != end_; ++it) {
      if (pred(*it)) {
        auto res = std::span{current_, it};
        current_ = it;
        return res;
      }
    }
    if (is_underlying_exhausted()) {
      // reached end and exhausted
      auto res = std::span(current_, end_);
      current_ = end_;
      assert(is_exhausted());
      return res;
    }
    // hit end of buffer, ran out of space. Move data to beginning

    auto it = move_to_begin();
    return next_until_impl(it, std::move(pred));
  }

private:
  template <typename Pred>
  constexpr std::span<value_type> next_until_impl(value_type* it, Pred pred) {
    for (; it != end_; ++it) {
      if (pred(*it)) {
        auto res = std::span(current_, it);
        current_ = it;
        return res;
      }
    }
    // exhausted, and buffer completely full, grow
    if (is_underlying_exhausted()) {
      // always check for exhaustion
      auto res = std::span(current_, end_);
      current_ = end_;
      assert(is_exhausted());
      return res;
    }
    // do growing
    grow_buffer(2 * buffer_capacity());
    return next_until_impl(current_, std::move(pred));
  }

  // returns current iterator
  constexpr value_type* move_to_begin() {
    assert(!is_underlying_exhausted() &&
           "No point in making space my moving to beginning if underlying is "
           "exhausted.");
    auto to_fill = std::move(current_, end_, buffer_.begin());
    current_ = buffer_.begin();
    auto end = fill_buffer(to_fill, buffer_.end());
    end_ = end;        // will not be equal buffer_.end() anymore if empty
    auto it = to_fill; // len from beginning, continue pred
    return it;
  }

  constexpr void grow_buffer(std::size_t capacity) {
    assert(!is_underlying_exhausted() &&
           "Why are you growing the buffer when the "
           "underlying stream is exhausted");
    std::size_t to_move = std::distance(current_, end_);
    value_type* to_fill{};
    buffer_.grow(capacity, [&](auto* dest) {
      to_fill = std::move(current_, end_, dest);
    });
    current_ = buffer_.begin();
    auto end = fill_buffer(to_fill, buffer_.end());
    end_ = end;
  }

  // fill buffer range with data. The caller is reponsible for handling any
  // implications from an exhausted underlying stream
  [[nodiscard]] constexpr value_type* fill_buffer(value_type* start,
                                                  value_type* end) {
    std::size_t read = stream_.next_n_into(start, std::distance(start, end));
    // will be less than distance(start, end) if empty
    return start + read;
  }
};

template <std::input_iterator It, std::sentinel_for<It> Sent>
class stream_range {
public:
  using value_type = std::iter_value_t<It>;
  using reference = std::iter_reference_t<It>;

private:
  [[no_unique_address]] It it_;
  [[no_unique_address]] Sent end_;

public:
  stream_range() = default;
  constexpr stream_range(It it, Sent end) : it_(it), end_(end) {}

  constexpr reference next() { return *it_++; }
  constexpr bool empty() const { return it_ == end_; }
};
} // namespace ely