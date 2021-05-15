#include "ely/string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ElyString ely_string_create_from_str(const char* __restrict__ str, size_t len)
{
    ElyString res = {
        .data = malloc(len),
        .len  = len,
    };
    memcpy(res.data, str, len);
    return res;
}

ElyString ely_string_create_from_zstr(const char* __restrict__ zstr)
{
    return ely_string_create_from_str(zstr, strlen(zstr));
}

void ely_string_destroy(ElyString* estr)
{
    free(estr->data);
}

ElyStringBuffer ely_string_buffer_clone(const ElyStringBuffer* to_clone)
{

    ElyStringBuffer res = {
        .data     = malloc(to_clone->capacity),
        .len      = to_clone->len,
        .capacity = to_clone->capacity,
    };

    __builtin_memcpy(res.data, to_clone->data, res.len);
    return res;
}

ElyStringBuffer ely_string_buffer_create()
{
    ElyStringBuffer res = {
        .data     = NULL,
        .len      = 0,
        .capacity = 0,
    };
    return res;
}

ElyStringBuffer ely_string_buffer_create_with_capacity(size_t capacity)
{
    ElyStringBuffer res = {
        .data     = malloc(capacity),
        .len      = 0,
        .capacity = capacity,
    };
    return res;
}

void ely_string_buffer_destroy(ElyStringBuffer* buffer)
{
    free(buffer->data);
}

static inline void string_buffer_grow(ElyStringBuffer* buffer,
                                      size_t           min_capacity)
{
#define max(a, b)                                                              \
    ({                                                                         \
        __typeof__(a) _a = (a);                                                \
        __typeof__(b) _b = (b);                                                \
        _a > _b ? _a : _b;                                                     \
    })

    size_t new_capacity = max(buffer->capacity * 2 + 1, min_capacity);
    char*  new_data     = realloc(buffer->data, new_capacity);
    assert(new_data);
    buffer->data     = new_data;
    buffer->capacity = new_capacity;

#undef max
}

void ely_string_buffer_append_str(ElyStringBuffer* buffer,
                                  const char* __restrict__ str,
                                  size_t len)
{
    if ((buffer->capacity - buffer->len) < len)
    {
        string_buffer_grow(buffer, buffer->len + len);
    }

    memcpy(buffer->data + buffer->len, str, len);
}

void ely_string_buffer_append_zstr(ElyStringBuffer* buffer,
                                   const char* __restrict__ zstr)
{
    return ely_string_buffer_append_str(buffer, zstr, strlen(zstr));
}

ElyString ely_string_buffer_into_string(ElyStringBuffer* buffer)
{
    char* new_str = realloc(buffer->data, buffer->len);
    assert(new_str);
    ElyString res = {
        .data = new_str,
        .len  = buffer->len,
    };

    buffer->data     = NULL;
    buffer->len      = 0;
    buffer->capacity = 0;

    return res;
}