#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string>

#include "ely/union.hpp"

class NoData
{};

using ely::destroy;
using ely::emplace;
using ely::get_unchecked;

TEST_CASE("Union")
{
    SUBCASE("empty optimization")
    {
        // this should be trivially contructible
        auto empty = ely::Union<>{};

        // checking whether a union without members takes up any space
        static_assert(std::is_empty_v<decltype(empty)>);

        // testing for EBO
        static_assert(std::is_empty_v<ely::Union<NoData>>);
        // testing for EBO with multiple same types
        static_assert(
            std::is_empty_v<ely::Union<NoData, NoData, NoData, NoData>>);
    }

    SUBCASE("default construct")
    {
        auto u = ely::Union<bool, int>{std::in_place_index<0>};

        REQUIRE_EQ(get_unchecked<0>(u), false);

        static_assert(std::is_trivially_copy_constructible_v<decltype(u)>);
        static_assert(std::is_trivially_copyable_v<decltype(u)>);
    }

    SUBCASE("trivial")
    {
        auto u = ely::Union<int, float, double>(std::in_place_index<1>, 5.f);
        using u_ty = decltype(u);

        static_assert(std::is_trivially_copy_constructible_v<u_ty>);
        static_assert(std::is_trivially_move_constructible_v<u_ty>);
        static_assert(std::is_trivially_copy_assignable_v<u_ty>);
        static_assert(std::is_trivially_move_assignable_v<u_ty>);
        static_assert(std::is_trivially_destructible_v<u_ty>);
        static_assert(
            std::is_trivial_v<u_ty>); // requires trivial default construct
    }

    SUBCASE("nontrivial")
    {
        auto u = ely::Union<int, std::string, float>(std::in_place_index<1>,
                                                     "Hello");

        static_assert(!std::is_trivially_copy_constructible_v<decltype(u)>);
        static_assert(!std::is_trivially_move_constructible_v<decltype(u)>);
        static_assert(!std::is_copy_constructible_v<decltype(u)>);

        static_assert(!std::is_trivial_v<decltype(u)>);
    }

    SUBCASE("usage")
    {
        // example usage with non-trivial types
        // this should be run with ubsan

        auto u = ely::Union<std::string, int, float, std::vector<int>>(
            std::in_place_index<0>, "Hello world");
        REQUIRE_EQ(get_unchecked<0>(u), "Hello world");
        static_assert(std::is_same_v<decltype(get_unchecked<1>(u)), int&>);

        destroy<0>(u); // need to be destroyed before emplacing another element
        emplace<1>(u, 10);

        REQUIRE_EQ(get_unchecked<1>(u), 10);

        // can emplace without destroy since int is trivially destructible
        emplace<3>(u, {1, 2, 3, 4, 5}); // initializer list constructor

        REQUIRE_EQ(get_unchecked<3>(u).size(), 5);
        destroy<3>(u);
    }
}