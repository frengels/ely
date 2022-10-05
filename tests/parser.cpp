#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

#include <mli/parser.hpp>

TEST_CASE("parser") {
  auto src = mli::string_view{"(hello world)\n123.43"};
  auto alloc = mli::arena{};
  auto p = mli::parser{src};

  auto tree = p.parse(alloc);

  auto l = tree.sexps()[0];
  auto num = tree.sexps()[1];

  CHECK(l->isa<mli::list>());
  CHECK(l->pos() == mli::source_offset{0});
  CHECK(num->isa<mli::decimal_literal>());
  CHECK(num->pos() == mli::source_offset{14});

  mli::arena_ptr<mli::list> l1 = l->get_if<mli::list>();
  CHECK(l1);
  CHECK(l1->sexps()[0]->isa<mli::identifier>());
  CHECK(l1->sexps()[1]->isa<mli::identifier>());

  mli::arena_ptr<mli::decimal_literal> dec =
      num->get_if<mli::decimal_literal>();
  CHECK(dec);
}