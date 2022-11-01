#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/arena.h>

TEST_CASE("arena")
{
    auto* a = ely_arena_create(1024 * 1024);

    SUBCASE("alloc")
    {
        void* p = ely_arena_allocate(a, sizeof(int), alignof(int));
        int*  i = static_cast<int*>(p);
        *i      = 5;

        CHECK_EQ(*i, 5);
    }

    ely_arena_release(a);
}