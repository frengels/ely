#pragma once

#include <algorithm>
#include <string>

namespace ely {

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_zstring_stream {
public:
  using char_type = CharT;

private:
  const char_type* str_;

public:
  explicit constexpr basic_zstring_stream(const CharT* str) : str_(str) {}

  constexpr const char_type* str() const { return str_; }
  constexpr const char_type* c_str() const { return str(); }

  constexpr char_type next() { return *str_++; }
  constexpr char_type peek() const { return *str_; }
};

using zstring_stream = basic_zstring_stream<char>;

template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_file_stream {
public:
  using char_type = CharT;

private:
  FILE* file_;

  char_type* buf_start_;
  char_type* buf_end_;
  char_type* buf_cur_;

public:
  basic_file_stream(FILE* f, char_type* buf, std::size_t buf_len)
      : file_(f), buf_start_(buf), buf_cur_(buf + buf_len),
        buf_end_(buf + buf_len) {}

  char_type peek() {
    if (buf_cur_ == buf_end_) {
      fill_buffer(buf_start_, buf_end_);
    }

    return *buf_cur_;
  }

  char_type next() { return next_n<1>()[0]; }

  template <std::size_t N> std::array<char_type, N> next_n() {
    // TODO: make this uninitialized
    std::array<char_type, N> result;

    if (buf_cur_ + N > buf_end_) {
      if constexpr (N == 1) {
        fill_buffer(buf_start_, buf_end_);
        std::copy(buf_cur_, buf_cur_ + N, result.begin());
        buf_cur_ += N;
      } else {
        std::copy(buf_cur_, buf_end_, result.begin());
        auto copied = std::distance(buf_cur_, buf_end_);
        fill_buffer(buf_start_, buf_end_);
        std::copy(buf_cur_, buf_cur_ + (N - copied), result.begin());
        buf_cur_ += (N - copied);
      }
    } else {
      std::copy(buf_cur_, buf_cur_ + N, result.data());
      buf_cur_ += N;
    }

    return result;
  }

private:
  void fill_buffer(char_type* start, char_type* end) {
    auto read =
        std::fread(start, sizeof(char_type), std::distance(start, end), file_);
    if (read < std::distance(start, end)) {
      *(start + read) = '\0';
    }

    buf_cur_ = start;
  }
};

using file_stream = basic_file_stream<char>;
} // namespace ely