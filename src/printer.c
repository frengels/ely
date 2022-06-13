#include "ely/printer.h"

#include <stdlib.h>

typedef struct ely_printer
{
    FILE* out;
} ely_printer;

ely_printer* ely_printer_create(FILE* out)
{
    ely_printer* p = malloc(sizeof(ely_printer));
    p->out         = out;
    return p;
}

void ely_printer_destroy(ely_printer* p)
{
    free(p);
}

void ely_printer_list(ely_printer* p, ely_list_type ty)
{}

void ely_printer_flush(ely_printer* p)
{
    fflush(p->out);
}