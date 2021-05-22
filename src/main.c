#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ely/ast.h>
#include <ely/lexer.h>
#include <ely/reader.h>

int main(int argc, char** argv)
{
    const char* filename = "test";
    const char* src = "(def (f x) #:x)\n123.123\n123\n123.\n123fd\n13.34fds\n";
    size_t      len = strlen(src);

    printf("source:\n%s\n", src);
    ElyToken     toks[32];
    ElyLexResult lex_res = ely_lex(src, len, toks, 32);
    uint32_t     read    = lex_res.tokens_read;
    printf("read %d tokens\n", read);

    for (int i = 0; i != read; ++i)
    {
        ElyToken      tok  = toks[i];
        ElyStringView strv = ely_token_as_pretty_string(tok.kind);
        printf("`%.*s`: %d\n", (int) strv.len, strv.data, tok.len);
    }

    ElyReader reader;
    ely_reader_create(&reader, filename);

    uint32_t i = 0;
    while (i != read)
    {
        ElyReadResult res  = ely_reader_read(&reader, src, toks + i, read - i);
        ElyNode*      node = res.node;
        i += res.tokens_consumed;
        if (node)
        {
            ElyString str = ely_node_to_string(node);
            printf("\nnode:\n%.*s\n", (int) str.len, str.data);
            ely_string_destroy(&str);
            printf("consumed %d tokens\n", res.tokens_consumed);
            ely_node_destroy(node);
        }
    }

    ElyList nodes;
    ely_list_create(&nodes);

    ely_reader_create(&reader, filename);
    ely_reader_read_all(&reader, src, toks, read, &nodes);

    // convert to ast

    ElyNode* head = ely_container_of(nodes.next, head, link);

    ElyExpr n = ely_ast_parse_expr(head);

    // perform cleanup
    ElyNode *e, *tmp;
    ely_list_for_each_safe(e, tmp, &nodes, link)
    {
        ElyString str = ely_node_to_string(e);
        printf("\n%.*s\n", (int) str.len, str.data);
        ely_string_destroy(&str);
        ely_node_destroy(e);
    }
}