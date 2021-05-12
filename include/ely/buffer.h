#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ely/defines.h"

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

#define ely_buffer_peek(ty, buffer) ((ty*) buffer->data)[buffer->begin]

static ELY_ALWAYS_INLINE void ely_buffer_advance(ElyBuffer* buffer)
{
    ++buffer->begin;
}

static ELY_ALWAYS_INLINE bool ely_buffer_empty(const ElyBuffer* buffer)
{
    return buffer->begin == buffer->end;
}