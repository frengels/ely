#include <ely/stx/scope.hpp>

#include <cassert>

template <ely::stx::scope_set SS> void test_scope() {
  auto gen = ely::stx::scope_generator();
  auto s = gen();

  auto a = SS().add_scope(s);
  auto b = a;
  assert(a.size() == 1);
  assert(b.size() == 1);

  assert(a == b);

  auto c = a.add_scope(gen());
  assert(c.size() == 2);
  assert(a == b);
  assert(a != c);
  assert(c != b);

  auto d = c;
  assert(c == d);
  assert(a.subset_of(c));
  assert(a.subset_of(d));
}

void scope() {
  test_scope<ely::stx::simple_scope_set>();
  fmt::println("ely/scope/simple - SUCCESS");
}

#ifndef NO_MAIN
int main() {
  scope();
  fmt::println("ely/scope - SUCCESS");
  return 0;
}
#endif
