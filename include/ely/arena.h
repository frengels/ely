#pragma once

#ifndef ELY_ARENA_H
#define ELY_ARENA_H

#include <stddef.h>

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_arena;

struct ely_arena_string
{
    const char* s;
    size_t      len;
};

struct ely_arena_alloc
{
    void*  data;
    size_t len;
};

ELY_EXPORT struct ely_arena* ely_arena_create(size_t initial_cap);

ELY_EXPORT void ely_arena_acquire(struct ely_arena* a);
ELY_EXPORT void ely_arena_release(struct ely_arena* a);

ELY_EXPORT void*
ely_arena_allocate(struct ely_arena* a, size_t size, size_t align);

ELY_EXPORT struct ely_arena_alloc
                ely_arena_alloc_maybe(const struct ely_arena* a, size_t align);
ELY_EXPORT void ely_arena_alloc_claim(struct ely_arena*      arena,
                                      struct ely_arena_alloc alloc,
                                      size_t                 size);

ELY_EXPORT struct ely_arena_string
ely_arena_allocate_string(struct ely_arena* a, const char* s);
ELY_EXPORT struct ely_arena_string
ely_arena_allocate_string_len(struct ely_arena* a, const char* s, size_t len);
#ifdef __cplusplus
}
#endif

#endif