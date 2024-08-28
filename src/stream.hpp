#pragma once

#include <array>
#include <cassert>
#include <filesystem>
#include <utility>

namespace ely {
class file_stream {
  friend class file_manager;

  FILE* file_;

  // not owning these
  char* buf_start_;
  char* buf_end_;
  char* buf_cur_;

public:
  file_stream(FILE* f, char* buf, std::size_t buf_len)
      : file_(f), buf_start_(buf), buf_cur_(buf), buf_end_(buf + buf_len) {}
  file_stream(const char* p, char* buf, std::size_t buf_len)
      : file_stream(std::fopen(p, "rb"), buf, buf_len) {}
  file_stream(const std::filesystem::path& p, char* buf, std::size_t buf_len)
      : file_stream(p.c_str(), buf, buf_len) {}

  file_stream(const file_stream&) = delete;
  file_stream& operator=(const file_stream&) = delete;

  file_stream(file_stream&& fs) noexcept
      : file_stream(fs.file_, fs.buf_start_,
                    std::distance(fs.buf_start_, fs.buf_end_)) {
    fs.file_ = nullptr;
    fs.buf_start_ = nullptr;
    fs.buf_end_ = nullptr;
    fs.buf_cur_ = nullptr;
  }

  file_stream& operator=(file_stream&& other) noexcept {
    close();

    file_ = std::exchange(other.file_, nullptr);
    buf_start_ = std::exchange(other.buf_start_, nullptr);
    buf_end_ = std::exchange(other.buf_end_, nullptr);
    buf_cur_ = std::exchange(other.buf_cur_, nullptr);

    return *this;
  }

  ~file_stream() { close(); }

  // returns true if the file is valid
  operator bool() const { return !file_; }

  char next() { return next<1>()[0]; }

  template <std::size_t Num> std::array<char, Num> next() {
    // TODO: make this uninitialized
    std::array<char, Num> result;

    // special case for 1 element
    if constexpr (Num == 1) {
    } else {
      if (buf_cur_ + Num > buf_end_) {
        std::copy(buf_cur_, buf_end_, result.data());
        auto copied = std::distance(buf_cur_, buf_end_);
        fill_buffer();
        std::copy(buf_cur_, buf_cur_ + (Num - copied), result.data());
        buf_cur_ += (Num - copied);
      } else {
        std::copy(buf_cur_, buf_cur_ + Num, result.data());
        buf_cur_ += Num;
      }
    }

    return result;
  }

  void close() {
    if (file_) {
      std::fclose(file_);
    }
  }

private:
  void fill_buffer() {
    auto read =
        std::fread(buf_start_, sizeof(char), buf_end_ - buf_start_, file_);
    if (read < std::distance(buf_start_, buf_end_)) {
      *(buf_start_ + read) = '\0';
    }

    buf_cur_ = buf_start_;
  }
};
} // namespace ely