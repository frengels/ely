#include <ely/green/list.hpp>
#include <ely/green/token.hpp>

#include "util.hpp"

using namespace ely::green;

constexpr int parser() {
  auto i = int_literal(0);
  auto s = string_literal("hello world");
  auto id = identifier("app");

  auto lb = list_builder{};
  lb.emplace_back(i);
  lb.emplace_back(std::move(s));

  auto lb2 = lb;
  auto l = lb.finish();

  check_eq("(0 \"hello world\")", fmt::to_string(l));

  lb2.emplace_back(id);
  l = lb2.finish();
  check_eq("(0 \"hello world\" app)", fmt::to_string(l));

  return 0;
}

#ifndef NO_MAIN
int main() {
  // unfortunately fmt is not all constexpr
  // static_assert(parser() == 0);
  return parser();
}
#endif