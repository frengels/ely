#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "ely/union.hpp"

class NoData
{};

TEST_CASE("Union")
{
    auto empty = ely::Union<>{};

    static_assert(std::is_empty_v<decltype(empty)>);

    SUBCASE("empty optimization")
    {
        // not yet implemented
        // static_assert(std::is_empty_v<ely::Union<NoData>>);
    }

    SUBCASE("default construct")
    {
        auto u = ely::Union<bool, int>{};

        REQUIRE_EQ(get_unchecked<0>(u), false);
    }

    SUBCASE("trivial")
    {
        auto u = ely::Union<int, float, double>(std::in_place_index<1>, 5.f);

        static_assert(std::is_trivial_v<decltype(u)>);
        static_assert(std::is_trivially_copy_constructible_v<decltype(u)>);
    }

    SUBCASE("nontrivial")
    {
        auto u = ely::Union<int, std::string, float>(std::in_place_index<1>,
                                                     "Hello");

        static_assert(!std::is_trivial_v<decltype(u)>);
        // copy must be allowed to make variant copy constructible as well
        static_assert(std::is_copy_constructible_v<decltype(u)>);
    }
}