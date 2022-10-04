#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

#include <mli/parser.hpp>

TEST_CASE("parser") {
  auto src = std::string_view{"(hello world)\n123.43"};
  auto alloc = mli::arena{};
  auto p = mli::parser{src};

  auto tree = p.parse(alloc);
}