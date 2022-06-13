#ifndef ELY_PRINTER_H
#define ELY_PRINTER_H

#include <stdio.h>

#include "ely/export.h"
#include "ely/stx/list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_printer ely_printer;

ELY_EXPORT ely_printer* ely_printer_create(FILE* out);
ELY_EXPORT void         ely_printer_destroy(ely_printer* p);

ELY_EXPORT void ely_printer_list(ely_printer* p, ely_list_type ty);
ELY_EXPORT void ely_printer_flush(ely_printer* p);

#ifdef __cplusplus
}
#endif

#endif