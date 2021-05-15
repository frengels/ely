#pragma once

#include <stdint.h>

#include "ely/defines.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline uint32_t ely_u32_count_numbers(uint32_t num)
{
    uint32_t count = 0;
    do
    {
        ++count;
        num /= 10;
    } while (num != 0);

    return count;
}

static inline uint32_t
ely_u32_write_to_buffer(uint32_t num, char* buffer, size_t buf_len)
{
    // 10 is the maximum characters output when converting
    char local_buffer[10];

    char*    p     = local_buffer;
    uint32_t count = 0;

    do
    {
        uint32_t remainder = num % 10;
        *p++               = char(remainder) + '0';
        num /= 10;
        ++count;
    } while (num > 0);

    do
    {
        *buffer++ = *--p;

    } while (p != local_buffer);

    return count;
}

#ifdef __cplusplus
}
#endif