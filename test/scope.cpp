#include <ely/scope.hpp>

#include <cassert>

#include "util.hpp"

template <typename SS> constexpr void test_scope() {
  auto gen = ely::scope_generator();
  auto s = gen();

  auto a = SS().add_scope(s);
  auto b = a;
  auto different = SS().add_scope(gen());
  assert(a.size() == 1);
  assert(b.size() == 1);
  assert(different.size() == 1);
  assert(a.subset_of(b));
  assert(b.subset_of(a));
  assert(!a.subset_of(different));
  assert(!different.subset_of(a));

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

  auto res = ely::resolver<std::string_view, int>{};
  assert(res.lookup("a", a).error() == ely::lookup_error::key_not_found);
  assert(res.insert("a", a, 0));

  assert(res.lookup("a", a).value() == 0);
  assert(res.lookup("a", different).error() ==
         ely::lookup_error::scope_not_found);

  assert(res.insert("a", c, 1));
  check_eq(res.lookup("a", a).value(), 0);
  check_eq(res.lookup("a", c).value(), 1);
}

void scope() {
  test_scope<ely::scope_set>();
  fmt::println("ely/scope/simple - SUCCESS");
}

#ifndef NO_MAIN
int main() {
  scope();
  fmt::println("ely/scope - SUCCESS");
  return 0;
}
#endif
