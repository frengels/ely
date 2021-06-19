#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "ely/union.hpp"
#include "ely/variant.hpp"

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

        REQUIRE_EQ(u.template get_unchecked<0>(), false);
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
        // can't copy because not all members are trivial
        static_assert(!std::is_copy_constructible_v<decltype(u)>);
    }
}

TEST_CASE("Variant")
{}