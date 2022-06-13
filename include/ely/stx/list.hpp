#ifndef ELY_STX_LIST_H
#define ELY_STX_LIST_H

#include <cstddef>
#include <cstdint>

#include <llvm/Support/SMLoc.h>

#include "ely/export.h"
#include "ely/ilist.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_datum ely_datum;

typedef enum ely_list_type
{
    ELY_LIST_PARENS,
    ELY_LIST_BRACKET,
    ELY_LIST_BRACE
} ely_list_type;

typedef struct ely_list
{
    ely_list_type type;
    llvm::SMRange range;
    ely_ilist     head;
} ely_list;

ELY_EXPORT void
ely_list_init(ely_list* list, ely_list_type ty, llvm::SMRange range);
ELY_EXPORT void ely_list_destroy(ely_list* list);

ELY_EXPORT uint32_t   ely_list_length(const ely_list* list);
ELY_EXPORT bool       ely_list_empty(const ely_list* list);
ELY_EXPORT void       ely_list_insert(ely_list* list, ely_datum* datum);
ELY_EXPORT void       ely_list_insert_list(ely_list* list, ely_list* other);
ELY_EXPORT ely_datum* ely_list_head(ely_list* list);

#ifdef __cplusplus
}
#endif

#endif