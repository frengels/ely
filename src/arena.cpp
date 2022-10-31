#include "ely/arena.h"

#include <algorithm>
#include <memory>

#include <cassert>
#include <cstddef>
#include <cstdint>

namespace
{
struct allocation
{
    void*  mem;
    size_t size;

    allocation(size_t sz) : mem(::operator new(sz)), size(sz)
    {}

    ~allocation()
    {
        ::operator delete(mem, size);
    }
};

struct arena_chunk
{
    allocation   alloc_;
    size_t       size_;
    arena_chunk* next{nullptr};

    arena_chunk(size_t cap) : alloc_(cap)
    {}

    constexpr const void* data() const
    {
        return alloc_.mem;
    }

    constexpr void* data()
    {
        return alloc_.mem;
    }

    constexpr size_t size() const
    {
        return size_;
    }

    constexpr size_t capacity() const
    {
        return alloc_.size;
    }

    constexpr size_t remaining() const
    {
        return capacity() - size();
    }

    void* alloc(size_t siz, size_t align)
    {
        ely_arena_alloc space = alloc_maybe(align);

        if (!space.data)
        {
            return nullptr;
        }

        if (space.len < siz)
        {
            return nullptr;
        }

        alloc_claim(space, siz);
        return space.data;
    }

    ely_arena_alloc alloc_maybe(size_t align) const
    {
        assert(align <= alignof(std::max_align_t) && "Too large alignment");
        size_t space = remaining();
        void*  p     = alloc_.mem; // we don't want alloc.mem to get changed
        // can be null if aligning takes us beyond space
        void* aligned_p = std::align(align, 0, p, space);

        if (aligned_p)
        {
            return ely_arena_alloc{aligned_p, space};
        }
        else
        {
            return ely_arena_alloc{};
        }
    }

    void alloc_claim(ely_arena_alloc alloc, std::size_t sz)
    {
        std::size_t distance = static_cast<std::byte*>(alloc.data) -
                               static_cast<std::byte*>(alloc_.mem);
        size_ = distance + sz;
    }
};
} // namespace

struct ely_arena
{
    std::uint32_t ref_count{1};
    arena_chunk   initial_;
    arena_chunk*  current_;

    ely_arena(size_t initial_cap) : initial_(initial_cap)
    {}

    ~ely_arena()
    {
        arena_chunk* it = &initial_;

        while (it)
        {
            arena_chunk* cur = it;
            it               = it->next;
            delete cur;
        }
    }

    void* alloc(size_t size, size_t align)
    {
        void* p = current_->alloc(size, align);

        if (p)
        {
            return p;
        }

        current_->next = new arena_chunk(std::max(current_->capacity(), size));
        current_       = current_->next;
        return current_->alloc(size, align);
    }

    ely_arena_alloc alloc_maybe(size_t align) const
    {
        return current_->alloc_maybe(align);
    }

    void alloc_claim(ely_arena_alloc alloc, size_t sz)
    {
        return current_->alloc_claim(alloc, sz);
    }
};

ely_arena* ely_arena_create(size_t initial_cap)
{
    return new ely_arena(initial_cap);
}

void ely_arena_acquire(ely_arena* a)
{
    ++a->ref_count;
}

void ely_arena_release(ely_arena* a)
{
    if (--a->ref_count == 0)
    {
        delete a;
    }
}

void* ely_arena_allocate(ely_arena* a, size_t size, size_t align)
{
    return a->alloc(size, align);
}

ely_arena_alloc ely_arena_alloc_maybe(const ely_arena* a, size_t align)
{
    return a->alloc_maybe(align);
}

void ely_arena_alloc_claim(ely_arena* a, ely_arena_alloc alloc, size_t sz)
{
    return a->alloc_claim(alloc, sz);
}

ely_arena_string ely_arena_allocate_string(ely_arena* a, const char* s)
{
    return ely_arena_allocate_string_len(a, s, __builtin_strlen(s));
}

ely_arena_string
ely_arena_allocate_string_len(ely_arena* a, const char* s, size_t len)
{
    void* alloc =
        ely_arena_allocate(a, (len + 1) * sizeof(char), alignof(char));
    char* alloc_s = static_cast<char*>(alloc);
    std::copy(s, s + len, alloc_s);
    alloc_s[len] = '\0';
    return {alloc_s, len};
}