#include <stdio.h>
#include <string.h>

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

    uint32_t offset = 0;
    for (uint32_t i = 0; i != read; ++i)
    {
        ElyToken tok = toks[i];
        if (tok.kind == ELY_TOKEN_NEWLINE_LF)
        {
            printf("%d `%s` \"\\n\"\n",
                   tok.len,
                   ely_token_as_pretty_string(tok.kind).data);
        }
        else
        {
            printf("%d `%s` \"%.*s\"\n",
                   tok.len,
                   ely_token_as_pretty_string(tok.kind).data,
                   tok.len,
                   &src[offset]);
        }
        offset += tok.len;
    }

    ElyReader reader;
    ely_reader_create(&reader, filename);

    ElyReadResult res   = ely_reader_read(&reader, lexer.src, toks, read);
    ElyNode*      plist = res.node;
}