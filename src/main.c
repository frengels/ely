#include <stdio.h>
#include <string.h>

#include <ely/reader.h>

int main(int argc, char** argv)
{
    const char* filename = "test";
    const char* src      = "(def (f x) x)";
    size_t      len      = strlen(src);

    ElyLexer lexer;
    ely_lex_create(&lexer, src, len);
    ElyToken toks[32];
    uint32_t read = ely_lex_src(&lexer, toks, 32);
    printf("read %d tokens\n", read);

    uint32_t offset = 0;
    for (uint32_t i = 0; i != read; ++i)
    {
        ElyToken tok = toks[i];
        printf("%d %.*s\n", tok.len, tok.len, &src[offset]);
        offset += tok.len;
    }

    ElyReader reader;
    ely_reader_create(&reader, filename);
}