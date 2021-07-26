#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "ely/simd.hpp"

#include <bitset>

TEST_CASE("simd")
{
    SUBCASE("recognize whitespace")
    {
        char*            src = new (static_cast<std::align_val_t>(64)) char[64];
        std::string_view src_unaligned = "  helloworld";
        std::memcpy(src, src_unaligned.data(), src_unaligned.size());

        auto str = ely::sse::u8x16::loadu(src);
        auto ws  = ely::sse::u8x16(' ');

        auto vals = (str == ws).as_bitmask();

        // leading zeroes
        REQUIRE_EQ(vals, 0b11);

        delete src;
    }

    SUBCASE("strings")
    {
        using u8chunk = ely::avx::u8x64;

        uint64_t even_mask = 0x5555'5555'5555'5555;
        uint64_t odd_mask  = 0xaaaa'aaaa'aaaa'aaaa;

        char* src = new (static_cast<std::align_val_t>(64)) char[128];
        // contains 2 strings "Hello world" & '''source "code"'''
        std::string_view src_unaligned =
            R"b(""\("Hello world" "source \"code \"\\")" hi again")b";
        std::memcpy(src, src_unaligned.data(), src_unaligned.size());
        src[src_unaligned.size()] = '\0';

        std::cout << "string: " << src << std::endl;

        auto strv            = u8chunk::loadu(src);
        auto backslash_maskv = u8chunk('\\');

        auto backslash_boolsv = strv == backslash_maskv;
        auto backslash       = backslash_boolsv.as_bitmask();

        std::cout << std::bitset<64>(backslash) << std::endl;

        auto backslash_starts = backslash & ~(backslash << 1);

        std::cout << "starts: " << std::bitset<64>(backslash_starts)
                  << std::endl;

        auto even_starts = backslash_starts & even_mask;
        std::cout << "even starts: " << std::bitset<64>(even_starts)
                  << std::endl;

        auto even_carries = even_starts + backslash;

        auto even_carries_only = even_carries & ~backslash;

        auto odd_starts = backslash_starts & odd_mask;

        auto odd_carries      = odd_starts + backslash;
        auto odd_carries_only = odd_carries & ~backslash;

        auto odd1 = even_carries_only & ~even_mask;
        auto odd2 = odd_carries_only & ~odd_mask;

        auto odd_backslash = odd1 | odd2;

        std::cout << "odd backslashes: " << std::bitset<64>(odd_backslash)
                  << std::endl;

        auto quote_mask = u8chunk('"');

        auto quote_bools = strv == quote_mask;
        auto quotes      = quote_bools.as_bitmask();

        std::cout << "quotes: " << std::bitset<64>(quotes) << std::endl;

        quotes &= ~odd_backslash;

        std::cout << "quotes: " << std::bitset<64>(quotes) << std::endl;

        auto quotes_v    = _mm_set_epi64x(static_cast<uint64_t>(0), quotes);
        auto str_range_v = _mm_clmulepi64_si128(quotes_v, _mm_set1_epi8(0xff), 0);

        alignas(16) uint64_t str_range;

        _mm_storel_epi64(reinterpret_cast<__m128i*>(&str_range), str_range_v);

        std::cout << "quoted ranges: " << std::bitset<64>(str_range) << std::endl;

        delete src;
    }
}