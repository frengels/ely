#include <ely/interner.hpp>

#include <cassert>
#include <fmt/core.h>
#include <tuple>

#include "support.hpp"

#include <string_view>

constexpr bool simple_interner() {
  auto interner = ely::simple_interner();

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

// template <typename T> bool test_interner2() {
//   auto intern = ely::interner2<T>{};
//   T d0 = intern.intern(4, 5, "Hello");
//   T d1 = intern.intern(4, 5, "Bye");

//   assert(d0 == d1);
//   assert(d0.name() == "Hello");
//   assert(d1.name() == "Hello");

//   T d2 = intern.intern(5, 4, "Yes");

//   assert(d2 != d0);

//   return true;
// }

void interner() {
  assert(simple_interner());
  static_assert(simple_interner());
  fmt::println("ely/simple_interner - SUCCESS");
}

#ifndef NO_MAIN
int main() {
  interner();

  return 0;
}
#endif
