#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "ely/union.hpp"
#include "ely/variant.hpp"

class NoData
{};

class Inheriting : public ely::variant::Variant<int, float, std::string>
{
    using base = ely::variant::Variant<int, float, std::string>;

    using base::base;
};

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
        // copy must be allowed to make variant copy constructible as well
        static_assert(std::is_copy_constructible_v<decltype(u)>);
    }
}

TEST_CASE("Variant")
{
    SUBCASE("single-entry")
    {
        // should be the same size as the underlying data
        auto v = ely::variant::Variant<bool>{};
        static_assert(sizeof(v) == sizeof(bool));
    }

    SUBCASE("construct")
    {
        auto v = ely::variant::Variant<int, float, std::string>{"hello world"};

        REQUIRE_EQ(v.index(), 2); // require std::string

        REQUIRE(visit(v, [](auto x) {
            if constexpr (std::is_same_v<std::string, decltype(x)>)
            {
                return x == "hello world";
            }
            else
            {
                return false;
            }
        }));
    }

    SUBCASE("construct in_place_type")
    {
        auto v = ely::variant::Variant<int, float, bool>(
            std::in_place_type<float>, 5);
        REQUIRE_EQ(v.index(), 1);

        SUBCASE("copy")
        {
            auto v2 = v;

            REQUIRE_EQ(v2.index(), v.index());

            v2 = std::move(v);
        }
    }

    SUBCASE("nontrivial")
    {
        auto v1 = ely::variant::Variant<std::string>("Hello world");

        REQUIRE_EQ(sizeof(v1), sizeof(std::string));

        SUBCASE("copy")
        {
            auto v2 = v1;

            REQUIRE_EQ(v2.index(), v1.index());

            v1 = std::move(v2);
        }
    }

    SUBCASE("inherit")
    {
        auto i = Inheriting("hello world");

        REQUIRE_EQ(i.index(), 2);
    }
}