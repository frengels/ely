#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/memory.hpp>

TEST_CASE("SharedPtr")
{
    auto p = ely::SharedPtr<int, int>{std::in_place, 5};

    REQUIRE_EQ(*p, 5);
    REQUIRE_EQ(p.ref_count(), 1);

    auto p2 = p;

    REQUIRE_EQ(*p2, 5);
    REQUIRE_EQ(p2.ref_count(), 2);

    auto p3 = std::move(p2);

    REQUIRE(!p2);

    REQUIRE_EQ(p3.ref_count(), 2);
    REQUIRE_EQ(*p3, 5);

    *p3 = 10;

    REQUIRE_EQ(*p, 10);
}