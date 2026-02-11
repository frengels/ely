#include <ely/stx/nodes.hpp>

#include "util.hpp"

constexpr int parser() {
  auto i = ely::stx::int_literal(0);
  auto s = ely::stx::string_literal("hello world");
  auto id = ely::stx::identifier("app");

  auto l = ely::stx::list{};
  l.emplace_back(i);
  l.emplace_back(std::move(s));

  check_eq("(0 \"hello world\")", fmt::to_string(l));

  l.emplace_back(id);

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