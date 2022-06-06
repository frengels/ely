#ifndef ELY_LOCATION_H
#define ELY_LOCATION_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_position
{
    uint32_t offset;
    uint32_t line;
    uint32_t col;
} ely_position;

typedef struct ely_location
{
    const char*  file;  // 8 bytes
    ely_position begin; // 20 bytes
    ely_position end;   // 32 bytes
} ely_location;

#ifdef __cplusplus
}
#endif

#endif