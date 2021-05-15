#pragma once

#include <assert.h>
#include <stddef.h>

#include "ely/defines.h"
#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyStringView   ElyStringView;
typedef struct ElyString       ElyString;
typedef struct ElyStringBuffer ElyStringBuffer;

typedef struct ElyStringView
{
    const char* data;
    size_t      len;
} ElyStringView;

typedef struct ElyString
{
    char*  data;
    size_t len;
} ElyString;

ELY_EXPORT ElyString ely_string_create_from_str(const char* __restrict__ str,
                                                size_t len);
ELY_EXPORT ElyString ely_string_create_from_zstr(const char* __restrict__ zstr);
ELY_EXPORT void      ely_string_destroy(ElyString* estr);

typedef struct ElyStringBuffer
{
    char*  data;
    size_t len;
    size_t capacity;
} ElyStringBuffer;

ELY_EXPORT ElyStringBuffer ely_string_buffer_create();
ELY_EXPORT ElyStringBuffer
ely_string_buffer_create_with_capacity(size_t capacity);
ELY_EXPORT ElyStringBuffer
                ely_string_buffer_clone(const ElyStringBuffer* to_clone);
ELY_EXPORT void ely_string_buffer_destroy(ElyStringBuffer* buffer);

ELY_ALWAYS_INLINE void
ely_string_buffer_append_str_unchecked(ElyStringBuffer* buffer,
                                       const char* __restrict__ str,
                                       size_t len)
{
    memcpy(buffer->data + buffer->len, str, len);
    buffer->len += len;
}
// append a zero-terminated string to buffer
ELY_EXPORT void ely_string_buffer_append_str(ElyStringBuffer* buffer,
                                             const char* __restrict__ str,
                                             size_t len);
ELY_EXPORT void ely_string_buffer_append_zstr(ElyStringBuffer* buffer,
                                              const char* __restrict__ zstr);
ELY_EXPORT void ely_string_buffer_reserve_unchecked(ElyStringBuffer* buffer,
                                                    size_t           capacity);
ELY_EXPORT void ely_string_buffer_reserve(ElyStringBuffer* buffer,
                                          size_t           capacity);

static ELY_ALWAYS_INLINE void
ely_string_buffer_append_char_unchecked(ElyStringBuffer* buffer, char ch)
{
    assert(buffer->len < buffer->capacity);
    buffer->data[buffer->len++] = ch;
}

ELY_EXPORT ElyString ely_string_buffer_into_string(ElyStringBuffer* buffer);

#ifdef __cplusplus
}
#endif
