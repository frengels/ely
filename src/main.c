#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ely/lexer.h>
#include <ely/reader.h>

int main(int argc, char** argv)
{
    const char* filename = "test";
    const char* src = "(def (f x) #:x)\n123.123\n123\n123.\n123fd\n13.34fds\n";
    size_t      len = strlen(src);

    printf("source:\n%s\n", src);
    ElyLexer lexer;
    ely_lexer_create(&lexer, src, len);
    ElyToken toks[32];
    uint32_t read = ely_lexer_lex(&lexer, toks, 32);
    printf("read %d tokens\n", read);

    ElyReader reader;
    ely_reader_create(&reader, filename);

    uint32_t i = 0;
    while (i != read)
    {
        ElyReadResult res =
            ely_reader_read(&reader, lexer.src, toks + i, read - i);
        ElyNode* plist = res.node;
        i += res.tokens_consumed;
        if (plist)
        {
            ElyString str = ely_node_to_string(plist);
            printf("\nnode:\n%.*s\n", (int) str.len, str.data);
            ely_string_destroy(&str);
            printf("consumed %d tokens\n", res.tokens_consumed);
            free(plist);
        }
    }
}