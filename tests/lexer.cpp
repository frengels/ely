#include <ely/lex/lexer.hpp>
#include <ely/lex/token.hpp>

#include <assert.h>
#include <stdio.h>

#define DEFINE_TEST_SINGLE_TOKEN(name, src, typ)                                \
    static inline void name()                                                   \
    {                                                                           \
        fprintf(stderr, "testing: " #name ", \"" src "\"\n");                   \
        ely::token<std::string_view> dst[16];                                   \
        auto                         lex = ely::lexer<std::string_view>((src)); \
        assert(!lex.empty());                                                   \
        auto dst_end = ely::copy(lex.begin(), lex.end(), dst);                  \
        auto write   = std::distance(dst, dst_end);                             \
        fmt::print("  wrote {} tokens\n", write);                               \
        fmt::print("  read {} characters\n", dst[0].size());                    \
        assert(write == 1);                                                     \
        fmt::print("  received token: {}\n", dst[0]);                           \
        assert(dst[0].kind() == typ);                                           \
    }

#define TOKENS_LEN 512

DEFINE_TEST_SINGLE_TOKEN(test_identifier, "hello", ely::token_type::identifier);
DEFINE_TEST_SINGLE_TOKEN(test_string_lit,
                         "\"hello\"",
                         ely::token_type::string_literal);
DEFINE_TEST_SINGLE_TOKEN(test_escaped_string_lit,
                         "\"hello\\\"\"",
                         ely::token_type::string_literal);
DEFINE_TEST_SINGLE_TOKEN(test_unterminated_string_lit,
                         "\"hello",
                         ely::token_type::unterminated_string_literal);
DEFINE_TEST_SINGLE_TOKEN(test_int, "12345", ely::token_type::int_literal);
DEFINE_TEST_SINGLE_TOKEN(test_dec,
                         "1234.1324321",
                         ely::token_type::decimal_literal);

static inline void test_eof(ely::token<std::string_view>* dst, uint32_t len)
{
    auto lex = ely::lexer<std::string_view>("");
    assert(lex.empty());
    auto dst_end = ely::copy(lex.begin(), lex.end(), dst);
    auto write   = std::distance(dst, dst_end);
    assert(write == 0);
    assert(lex.empty());
}

int main(int argc, char** argv)
{
    ely::token<std::string_view> dst[TOKENS_LEN];

    const uint32_t len = TOKENS_LEN;

    test_eof(dst, len);
    test_identifier();
    test_string_lit();
    test_escaped_string_lit();
    test_unterminated_string_lit();
    test_int();
    test_dec();

    return 0;
}