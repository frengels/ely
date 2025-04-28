#include <cassert>
#include <fmt/core.h>

#include <algorithm>
#include <numeric>

#include <ely/arena/bump.hpp>
#include <ely/arena/forever.hpp>
#include <ely/arena/shared.hpp>
#include <ely/arena/string.hpp>

struct not_simple {
  int val;

  not_simple() : val(5) {}

  operator int() const { return val; }
};

template <typename Arena> void test_arena() {
  auto a = Arena{};
  auto p = a.template make<int>(5);
  assert(p);
  assert(*p == 5);
}

template <typename Arena, typename T> void test_array_arena() {
  static_assert(std::is_trivial_v<T>, "T is not trivial.");

  constexpr int arr_size = 1000;
  auto a = Arena();
  // create and zero out
  typename Arena::ptr_type<T[]> arr = a.template make<T[]>(arr_size);
  // initialized to 0
  std::for_each(std::to_address(arr), std::to_address(arr) + arr_size,
                [](const T& i) { assert(i == T{}); });
  std::iota(std::to_address(arr), std::to_address(arr) + arr_size, T{});

  T count{};
  std::for_each(std::to_address(arr), std::to_address(arr) + arr_size,
                [&](const T& i) { assert(i == count++); });

  std::span<T> copied =
      a.copy(std::to_address(arr), std::to_address(arr) + arr_size);
  count = {};
  std::for_each(copied.begin(), copied.end(),
                [&](const T& i) { assert(i == count++); });
}

template <typename Arena, typename T> void test_array_arena_nontriv() {
  static_assert(!std::is_trivial_v<T>, "T cannot be trivial.");

  constexpr int arr_size = 1000;
  auto a = Arena();

  typename Arena::ptr_type<T[]> arr = a.template make<T[]>(arr_size);

  std::for_each(std::to_address(arr), std::to_address(arr) + arr_size,
                [](const T& v) { assert(v == T{}); });
}

void forever_arena() {
  test_arena<ely::arena::forever>();
  fmt::println("ely/arena/forever - SUCCESS");

  test_array_arena<ely::arena::forever, int>();
  fmt::println("ely/arena/array/forever/int - SUCCESS");
  test_array_arena<ely::arena::forever, float>();
  fmt::println("ely/arena/array/forever/float - SUCCESS");
  test_array_arena_nontriv<ely::arena::forever, not_simple>();
  fmt::println("ely/arena/array/forever/not_simple - SUCCESS");
}

void shared_arena() {
  test_arena<ely::arena::shared>();
  fmt::println("ely/arena/shared - SUCCESS");

  // disabled due to shared_ptr not implementing operator-> for to_address
  // test_array_arena<ely::arena::shared>();
  // fmt::println("ely/arena/array/shared - SUCCESS");
}

void bump_arena() {
  test_arena<ely::arena::bump>();
  fmt::println("ely/arena/bump - SUCCESS");

  test_array_arena<ely::arena::bump, int>();
  fmt::println("ely/arena/array/bump/int - SUCCESS");

  test_array_arena<ely::arena::bump, float>();
  fmt::println("ely/arena/array/bump/float - SUCCESS");
}

void arena() {
  shared_arena();
  forever_arena();
  bump_arena();

  fmt::println("ely/arena - SUCCESS");
}

#ifndef NO_MAIN
int main() {
  arena();

  return 0;
}
#endif
