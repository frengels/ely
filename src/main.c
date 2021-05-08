#include <stdio.h>
#include <string.h>

#include <ely/reader.h>

int main(int argc, char** argv)
{
    const char* filename = "test";
    const char* src      = "(def (f x) x)";
    size_t      len      = strlen(src);

    struct ElyLexer lexer;
    ely_lex_create(&lexer, src, len);
    struct ElyToken toks[32];
    uint32_t        read = ely_lex_src(&lexer, toks, 32);
    printf("read %d tokens\n", read);

    struct ElyReader reader;
    ely_reader_create(&reader, filename);
}