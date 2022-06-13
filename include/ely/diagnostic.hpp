#ifndef ELY_DIAGNOSTIC_H
#define ELY_DIAGNOSTIC_H

#include <stdint.h>

#include "ely/location.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ELY_DIAGNOSTIC_NOTE,
    ELY_DIAGNOSTIC_WARNING,
    ELY_DIAGNOSTIC_ERROR,
} ely_diagnostic_type;

typedef struct
{
    ely_diagnostic_type type;
    ely_location        loc;
    const char*         msg;
} ely_diagnostic;

#ifdef __cplusplus
}
#endif

#endif
