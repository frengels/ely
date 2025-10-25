#include <ely/interner.hpp>

#include <cassert>
#include <fmt/core.h>

#include "support.hpp"

#include "ely/arena/block.hpp"

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

void interner() {
  assert(simple_interner());
  fmt::println("ely/simple_interner - SUCCESS");
}

#ifndef NO_MAIN
int main() {
  interner();

  return 0;
}
#endif
