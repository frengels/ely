#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <type_traits>

#include <ely/pair.hpp>

struct Empty1
{
    Empty1() = default;

    Empty1(int i)
    {}
};
struct Empty2
{
    Empty2() = default;

    Empty2(float f)
    {}
};

TEST_CASE("pair")
{
    SUBCASE("empty")
    {
        SUBCASE("default")
        {
            auto p = ely::EBOPair<Empty1, Empty2>{};

            static_assert(std::is_empty_v<decltype(p)>);
            static_assert(std::is_trivial_v<decltype(p)>);
        }

        SUBCASE("construct")
        {
            auto p = ely::EBOPair<Empty1, Empty2>{5, 5.f};
        }
    }

    SUBCASE("empty same")
    {
        auto p = ely::EBOPair<Empty1, Empty1>{5, 5};

        static_assert(std::is_empty_v<decltype(p)>);
        static_assert(std::is_trivial_v<decltype(p)>);
    }

    SUBCASE("half empty")
    {
        auto p = ely::EBOPair<Empty1, int>{};

        static_assert(sizeof(p) == sizeof(int));

        REQUIRE_EQ(p.second(), 0);
    }
}