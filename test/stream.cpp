#include <ely/stream.hpp>

#include <ely/buffer.hpp>

#include <cassert>

struct a_stream {
  using value_type = char;
  static constexpr value_type empty_value = '\0';
  std::size_t count = 10000;

  constexpr char next() {
    char res;
    next_n_into(&res, 1);
    return res;
  }

  constexpr std::size_t next_n_into(char* out, std::size_t buf_len) {
    auto n = std::min(count, buf_len);
    std::for_each_n(out, n, [](char& c) { c = 'a'; });
    if (n < buf_len)
      out[n] = '\0';
    count -= n;
    return n;
  }
};

constexpr int stream() {
  {
    auto buffer = ely::alloc_buffer<char>{8};
    auto astream = ely::buffered_stream(a_stream{}, std::move(buffer));

    char a{};
    std::size_t count = 0;
    do {
      a = astream.next();
      ++count;
    } while (a != '\0');
    --count; // subtract for \0
    assert(count == 10000);
  }
  {
    auto buffer = ely::alloc_buffer<char>{8};
    auto astream = ely::buffered_stream(a_stream{}, std::move(buffer));
    // ask for a too large next
    constexpr std::size_t read_size = 32;
    std::size_t as_size{};
    std::size_t count{};
    do {
      auto as = astream.next_n(read_size);
      as_size = as.size();
      for (char x : as) {
        assert(x == 'a');
        ++count;
      }
    } while (as_size == read_size);
    assert(count == 10000);
  }
  {
    auto buffer = ely::alloc_buffer<char>{8};
    auto astream = ely::buffered_stream(a_stream{}, std::move(buffer));
    auto res = astream.next_until([](char x) { return x == 'b'; });
    assert(res.size() == 10000);
  }
  return 0;
}

#ifndef NO_MAIN
int main() {
  static_assert(stream() == 0);
  return stream();
}
#endif
