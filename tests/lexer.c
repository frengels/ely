#include <ely/lex/lexer.h>
#include <ely/lex/token.h>

#include <assert.h>

#define TOKENS_LEN 512

static inline void test_identifier(ely_token* dst, uint32_t len)
{
    ely_lexer* lex = ely_lexer_create("hello");

    assert(!ely_lexer_empty(lex));
    uint32_t write = ely_lexer_scan_tokens(lex, dst, len);
    assert(write == 1);
    assert(dst[0].type == ELY_TOKEN_IDENTIFIER);
    assert(ely_lexer_empty(lex));

    ely_lexer_destroy(lex);
}

static inline void test_eof(ely_token* dst, uint32_t len)
{
    ely_lexer* lex = ely_lexer_create("");
    assert(ely_lexer_empty(lex));
    uint32_t write = ely_lexer_scan_tokens(lex, dst, len);
    assert(write == 0);
    assert(ely_lexer_empty(lex));

    ely_lexer_destroy(lex);
}

static inline void test_string_lit(ely_token* dst, uint32_t len)
{
    ely_lexer* lex = ely_lexer_create("\"hello\"" "\"test escape\\\"\"");
    assert(!ely_lexer_empty(lex));
    uint32_t write = ely_lexer_scan_tokens(lex, dst, len);

    assert(write == 2);
    assert(dst[0].type == ELY_TOKEN_STRING);
    assert(dst[1].type == ELY_TOKEN_STRING);
}

int main(int argc, char** argv)
{
    ely_token dst[TOKENS_LEN];

    const uint32_t len = TOKENS_LEN;

    test_identifier(dst, len);
    test_eof(dst, len);
    test_string_lit(dst, len);

    return 0;
}