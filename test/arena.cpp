#include "ely/arena/constexpr.hpp"
#include "ely/arena/slab.hpp"

#include <cstring>
#include <fmt/core.h>
#include <type_traits>

template <typename Arena> constexpr bool test_arena() {
  Arena arena;
  auto* p1 = arena.template allocate<int>();
  auto* p2 = arena.template allocate<double>();
  auto* p3 = arena.template allocate<char>();

  *p1 = 42;
  *p2 = 3.14;
  *p3 = 'x';

  // the following checks can't happen at compile time
  if (!std::is_constant_evaluated()) {
    auto* bytes = arena.allocate_bytes(16, 4096);
    assert(reinterpret_cast<std::uintptr_t>(bytes) % 4096 == 0);
  }

  assert(*p1 == 42);
  assert(*p2 == 3.14);
  assert(*p3 == 'x');

  constexpr std::string_view hello_world = "hello world";

  // try out more complex types and arrays
  auto* arr = arena.template allocate<char>(hello_world.size() + 1);
  std::copy(hello_world.begin(), hello_world.end(), arr);
  arr[hello_world.size()] = '\0';

  std::string* str = arena.template allocate<std::string>();
  std::construct_at(str);
  *str = hello_world.data();

  assert(std::string_view(arr) == std::string_view(*str));

  return true;
}

void arena() {
  assert(test_arena<ely::arena::slab<4096>>());
  static_assert(test_arena<ely::arena::constexpr_>());
}

#ifndef NO_MAIN
int main() {
  arena();
  fmt::println("ely/arena - SUCCESS");
  return 0;
}
#endif
