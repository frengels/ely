#ifndef ELY_LOCATION_H
#define ELY_LOCATION_H

#include <stdint.h>

#include "ely/position.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_location
{
    const char*   file;  // 8 bytes
    ely::position begin; // 20 bytes
    ely::position end;   // 32 bytes
} ely_location;

#ifdef __cplusplus
}
#endif

#endif