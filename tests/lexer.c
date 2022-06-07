#include <ely/lex/lexer.h>
#include <ely/lex/token.h>

#include <assert.h>
#include <stdio.h>

#define DEFINE_TEST_SINGLE_TOKEN(name, src, ty)                                \
    static inline void name()                                                  \
    {                                                                          \
        fprintf(stderr, "testing: " #name ", \"" src "\"\n");                  \
        ely_token    dst[16];                                                  \
        ely_lexer*   lex   = ely_lexer_create((src));                          \
        ely_position start = ely_lexer_position(lex);                          \
        assert(!ely_lexer_empty(lex));                                         \
        uint32_t write = ely_lexer_scan_tokens(lex, dst, 16);                  \
        fprintf(stderr, "  wrote %d tokens\n", write);                         \
        ely_position end = ely_lexer_position(lex);                            \
        fprintf(stderr, "  read %d characters\n", end.offset - start.offset);  \
        assert(write == 1);                                                    \
        fprintf(stderr,                                                        \
                "  received token: \"%s\"\n",                                  \
                ely_token_type_to_string(dst[0].type));                        \
        assert(dst[0].type == ty);                                             \
        ely_lexer_destroy(lex);                                                \
    }

#define TOKENS_LEN 512

DEFINE_TEST_SINGLE_TOKEN(test_identifier, "hello", ELY_TOKEN_IDENTIFIER);
DEFINE_TEST_SINGLE_TOKEN(test_string_lit, "\"hello\"", ELY_TOKEN_STRING);
DEFINE_TEST_SINGLE_TOKEN(test_escaped_string_lit,
                         "\"hello\\\"\"",
                         ELY_TOKEN_STRING);
DEFINE_TEST_SINGLE_TOKEN(test_unterminated_string_lit,
                         "\"hello",
                         ELY_TOKEN_UNTERMINATED_STRING);
DEFINE_TEST_SINGLE_TOKEN(test_int, "12345", ELY_TOKEN_INT);
DEFINE_TEST_SINGLE_TOKEN(test_dec, "1234.1324321", ELY_TOKEN_DEC);

static inline void test_eof(ely_token* dst, uint32_t len)
{
    ely_lexer* lex = ely_lexer_create("");
    assert(ely_lexer_empty(lex));
    uint32_t write = ely_lexer_scan_tokens(lex, dst, len);
    assert(write == 0);
    assert(ely_lexer_empty(lex));

    ely_lexer_destroy(lex);
}

int main(int argc, char** argv)
{
    ely_token dst[TOKENS_LEN];

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