#include <ely/lex/lexer.hpp>
#include <ely/lex/token.hpp>

#include <assert.h>
#include <stdio.h>

#define DEFINE_TEST_SINGLE_TOKEN(name, src, ty)                                \
    static inline void name()                                                  \
    {                                                                          \
        fprintf(stderr, "testing: " #name ", \"" src "\"\n");                  \
        ely::token   dst[16];                                                  \
        ely::lexer   lex   = ely::lexer((src));                                \
        ely_position start = lex.position();                                   \
        assert(!lex.empty());                                                  \
        uint32_t write = lex.scan_tokens(dst, 16);                             \
        fprintf(stderr, "  wrote %d tokens\n", write);                         \
        ely_position end = lex.position();                                     \
        fprintf(stderr, "  read %d characters\n", end.offset - start.offset);  \
        assert(write == 1);                                                    \
        fprintf(stderr,                                                        \
                "  received token: \"%s\"\n",                                  \
                ely::token_type_to_string(dst[0].type));                       \
        assert(dst[0].type == ty);                                             \
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
                         ely::token_type::unterminated_string);
DEFINE_TEST_SINGLE_TOKEN(test_int, "12345", ely::token_type::int_literal);
DEFINE_TEST_SINGLE_TOKEN(test_dec,
                         "1234.1324321",
                         ely::token_type::decimal_literal);

static inline void test_eof(ely::token* dst, uint32_t len)
{
    ely::lexer lex = ely::lexer("");
    assert(lex.empty());
    uint32_t write = lex.scan_tokens(dst, len);
    assert(write == 0);
    assert(lex.empty());
}

int main(int argc, char** argv)
{
    ely::token dst[TOKENS_LEN];

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