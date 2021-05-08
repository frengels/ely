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
    printf("read %d tokens\n", read);

    ElyReader reader;
    ely_reader_create(&reader, filename);
}