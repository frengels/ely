#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <ely/util.h>

TEST_CASE("util")
{
    SUBCASE("u32toa")
    {
        SUBCASE("nonzero")
        {
            char buffer[10];

            uint32_t written = ely_u32_write_to_buffer(1234, buffer);

            REQUIRE_EQ(4, ely_u32_count_numbers(1234));

            REQUIRE(written == 4);
            REQUIRE(buffer[0] == '1');
            REQUIRE(buffer[1] == '2');
            REQUIRE(buffer[2] == '3');
            REQUIRE(buffer[3] == '4');
        }
        SUBCASE("zero")
        {
            char zero;

            uint32_t written = ely_u32_write_to_buffer(0, &zero);

            REQUIRE_EQ(1, ely_u32_count_numbers(0));

            REQUIRE(written == 1);
            REQUIRE(zero == '0');
        }
    }
}