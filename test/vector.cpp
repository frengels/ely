#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/vector.hpp>

TEST_CASE("Vector")
{
    static_assert(std::is_trivially_destructible_v<ely::StaticVector<int, 64>>);
    static_assert(sizeof(ely::StaticVector<int, 64>) == sizeof(int) * 65);

    static_assert(
        !std::is_trivially_destructible_v<ely::StaticVector<std::string, 1>>);

    SUBCASE("constructor")
    {
        auto sv = ely::StaticVector<int, 8>{};

        REQUIRE_EQ(sv.size(), 0);
        REQUIRE_EQ(sv.capacity(), 8);

        for (int i = 0; i < sv.capacity(); ++i)
        {
            sv.emplace_back(i);
        }

        REQUIRE_EQ(sv.size(), 8);

        for (int i = 0; i < sv.size(); ++i)
        {
            REQUIRE_EQ(sv[i], i);
        }
    }
}