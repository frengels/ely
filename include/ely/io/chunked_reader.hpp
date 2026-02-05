#pragma once

#include <cstdio>
#include <span>
#include <string>

namespace ely {
namespace io {
// very simple buffered reader, will simply return the whole buffer on each
// read, and refill the buffer when it's empty. This is not intended to be a
// general purpose buffered reader, just something to make it easier to
// implement the lexer and parser without having to worry about buffering logic.
// It will not handle seeking or anything like that, and it will not handle
// multiple buffers or anything like that. It will simply read into a single
// buffer and return that buffer on each read until the underlying stream is
// exhausted.
class chunked_reader {
  FILE* fd_;
  char* buffer_;
  std::size_t buffer_size_;

public:
  explicit chunked_reader(FILE* fd) : fd_(fd) {
    buffer_size_ = 32 * 1024; // 32KB buffer
    buffer_ = new char[buffer_size_];
  }

  ~chunked_reader() {
    if (buffer_)
      delete[] buffer_;
  }

  std::string_view next() {
    std::size_t read = std::fread(buffer_, sizeof(char), buffer_size_, fd_);
    return std::string_view(buffer_, read);
  }
};
} // namespace io
} // namespace ely