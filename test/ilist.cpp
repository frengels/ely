#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/ilist.hpp>

class Hooked : public ely::IListBaseHook<>
{
public:
    int i;

public:
    constexpr Hooked(int i) : i(i)
    {}
};

TEST_CASE("ilist")
{
    ely::IList<Hooked> list{};

    SUBCASE("insert")
    {
        Hooked h1{1};
        Hooked h2{2};
        Hooked h3{3};

        list.insert_back(h1);
        list.insert_back(h2);
        list.insert_back(h3);

        auto it = list.begin();
        REQUIRE_EQ(it++->i, 1);
        REQUIRE_EQ(it++->i, 2);
        REQUIRE_EQ(it++->i, 3);
        REQUIRE_EQ(it, list.end());

        SUBCASE("removal")
        {}
    }
}