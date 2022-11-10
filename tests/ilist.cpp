#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/ilist.hpp>

struct intrusive_element : private ely::ilink
{
    friend ely::ilink_access;

    constexpr intrusive_element(int i, float f) : i(i), f(f)
    {}

    int   i;
    float f;
};

TEST_CASE("ilist")
{
    auto il = ely::ilist<intrusive_element>{};

    intrusive_element el1{5, 5.f};
    intrusive_element el2{10, 0.f};

    SUBCASE("insert")
    {
        il.push_back(el1);
        il.push_back(el2);

        CHECK_EQ(il.count_size(), 2);

        auto it = il.begin();
        CHECK_EQ(it->i, el1.i);
        CHECK_EQ(it->f, el1.f);

        auto it2 = std::next(it);
        CHECK_EQ(it2->i, el2.i);
        CHECK_EQ(it2->f, el2.f);

        it2.erase();
        CHECK_EQ(il.count_size(), 1);

        it2 = std::next(it);
        CHECK_EQ(it2, il.end());
    }
}