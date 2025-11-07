#include <ely/util/dispatch_index.hpp>
#include <ely/util/variant.hpp>

struct empty_t {};

constexpr bool test_dispatch() {
  auto i = ely::dispatch_index<10>(
      5, []<std::size_t I>(std::in_place_index_t<I>) { return I; });
  assert(i == 5);
  return true;
}

void variant() {

  assert(test_dispatch());
  static_assert(test_dispatch());
  // this should really be equal in size as it doesn't require an index
  static_assert(sizeof(ely::variant<int>) == sizeof(int));

  static_assert(std::is_empty_v<empty_t>);
  // TODO doesn't work with std::tuple's implementation
  // static_assert(std::is_empty_v<std::variant<empty_t>>);
}

#ifndef NO_MAIN
int main() {
  variant();
  return 0;
}
#endif
