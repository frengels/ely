#include "elyc/lexer.h"

uint32_t elyc_lex_src(struct ElycLexer* lex,
                  const char* __restrict__ src,
                  uint32_t src_len,
                  struct ElycToken* __restrict__ token_buf,
                  uint32_t buf_len)
{
    if (buf_len == 0 || src_len == 0)
    {
        return 0;
    }

    char     ch  = src[0];
    uint32_t len = 1;
    --src_len;

    switch (ch)
    {
    case ' ':
        for (; src_len != 0; --src_len)
        {
            ch = src[len];

            if (ch == ' ')
            {
                ++len;
            }
            else
            {
                token_buf[0].kind = ELY_TOKEN_WHITESPACE;
                token_buf[0].len  = len;

                __attribute__((musttail)) return elyc_lex_src(
                    lex, &src[len], src_len, &token_buf[1], buf_len - 1);
            }
        }

        token_buf[0].kind = ELY_TOKEN_EOF;
        token_buf[0].len = 0;
        return 1;
    case '\t':
    case '(':

    case ')':
    case '[':
    case ']':
    case '{':
    case '}':
    default:
        __builtin_unreachable();
    }

    return 0;
}