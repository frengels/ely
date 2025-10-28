#include <ely/interner.hpp>

#include <cassert>
#include <fmt/core.h>
#include <tuple>

#include "support.hpp"

#include "ely/arena/block.hpp"

#include <string_view>

class data {
private:
  struct data_storage {
    int x, y;
    std::string name;

    using key_type = std::tuple<int, int>;

  public:
    data_storage(int x, int y, std::string name)
        : x(x), y(y), name(std::move(name)) {}

    static key_type get_key(int x, int y, const std::string&) {
      return key_type{x, y};
    }
  };

public:
  using storage_type = data_storage;

private:
  storage_type* impl_;

public:
  data() = default;
  // construct from store pointer
  constexpr data(storage_type* store) : impl_(store) {}

  // simple pointer equality
  friend constexpr bool operator==(const data& lhs, const data& rhs) {
    return lhs.impl_ == rhs.impl_;
  }

  constexpr int x() const { return impl_->x; }
  constexpr int y() const { return impl_->y; }
  constexpr std::string_view name() const { return impl_->name; }
};

bool simple_interner() {
  auto arena = ely::arena::fixed_block<char, 128 * 1024>{};
  auto interner = ely::simple_interner(arena);

  // 2 equivalent interned symbol must compare equal
  auto hello_sym = interner.intern("hello");
  auto hello2_sym = interner.intern("hello");
  assert(hello_sym == hello2_sym);

  // must compare not equal
  auto foo_sym = interner.intern("foo");
  auto bar_sym = interner.intern("bar");
  assert(foo_sym != bar_sym);

  // must be able to lookup
  assert(interner.lookup(foo_sym) == "foo");
  assert(interner.lookup(bar_sym) == "bar");
  assert(interner.lookup(hello_sym) == "hello");

  return true;
}

template <typename T> bool test_interner2() {
  auto intern = ely::interner2<T>{};
  T d0 = intern.intern(4, 5, "Hello");
  T d1 = intern.intern(4, 5, "Bye");

  assert(d0 == d1);
  assert(d0.name() == "Hello");
  assert(d1.name() == "Hello");

  T d2 = intern.intern(5, 4, "Yes");

  assert(d2 != d0);

  return true;
}

void interner() {
  assert(simple_interner());
  fmt::println("ely/simple_interner - SUCCESS");
  assert(test_interner2<data>());
}

#ifndef NO_MAIN
int main() {
  interner();

  return 0;
}
#endif
