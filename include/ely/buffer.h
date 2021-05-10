#pragma once

#include <stdint.h>

/// size and capacity should be according to the callee's specified format
/// data is expected to be cast to the target type before usage
typedef struct ElyBuffer
{
    void* __restrict__ data;
    // the first "active" element
    uint32_t begin;
    // one after the last "active" element, end is always >= begin
    uint32_t end;
    // the total capacity of data
    uint32_t capacity;
} ElyBuffer;