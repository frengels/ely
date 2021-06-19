#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "ely/storage.hpp"

TEST_CASE("Storage")
{
    SUBCASE("Stack")
    {
        auto cont = ely::storage::Stack<int, 32>{};

        REQUIRE_EQ(cont.size(), 0);
        REQUIRE_EQ(cont.capacity(), 32);

        for (auto i = 0; i != 32; ++i)
        {
            cont.emplace_back(i);
        }

        REQUIRE_EQ(cont.size(), 32);
    }
}