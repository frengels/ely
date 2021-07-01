#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/variant.hpp>

class Inheriting : public ely::Variant<int, float, std::string>
{
    using base = ely::Variant<int, float, std::string>;

    using base::base;
};

TEST_CASE("Variant")
{
    SUBCASE("single-entry")
    {
        // should be the same size as the underlying data
        auto v = ely::Variant<bool>{};
        static_assert(sizeof(v) == sizeof(bool));
    }

    SUBCASE("construct")
    {
        auto v = ely::Variant<int, float, std::string>{"hello world"};

        using v_ty = decltype(v);

        static_assert(std::is_copy_constructible_v<v_ty>);
        static_assert(std::is_move_constructible_v<v_ty>);
        static_assert(std::is_copy_assignable_v<v_ty>);
        static_assert(std::is_move_assignable_v<v_ty>);

        REQUIRE_EQ(v.index(), 2); // require std::string

        REQUIRE(visit(
            [](auto x) {
                if constexpr (std::is_same_v<std::string, decltype(x)>)
                {
                    return x == "hello world";
                }
                else
                {
                    return false;
                }
            },
            v));

        auto v2 = v;

        REQUIRE_EQ(v.index(), v2.index());
    }

    SUBCASE("construct in_place_type")
    {
        auto v = ely::Variant<int, float, bool>(std::in_place_type<float>, 5);
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
        auto v1 = ely::Variant<std::string>("Hello world");

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