#pragma once

#include <memory>

#include <cstdint>

#include "ely/ilist.hpp"

namespace ely
{
enum struct node_kind : std::uint8_t
{
    stx_id_user,
    stx_id_prim,

    stx_list,

    stx_lit_int,
    stx_lit_dec,
    stx_lit_str,

    stx_lit_i32,
    stx_lit_i64,
    stx_lit_u32,
    stx_lit_u64,
    stx_lit_f32,
    stx_lit_f64,

    call_user,
    call_prim,
};

class node_base : private ely::ilink
{
    friend ely::ilink_access;

private:
    node_kind kind_;

public:
    constexpr node_base(node_kind kind) : kind_(kind)
    {}

    constexpr node_kind kind() const
    {
        return kind_;
    }
};

template<typename T>
class node_ptr
{
    node_base* base;
    // followed by the actual node data

public:
    constexpr node_ptr(node_base* base) : base(base)
    {}
};

template<typename T>
struct node_traits
{
    static constexpr node_kind kind = T::node_kind;
};

template<typename T, typename Alloc, typename... Args>
node_ptr<T> create(Alloc& alloc_, Args&&... args)
{
    using alloc_traits = std::allocator_traits<Alloc>;
    using char_alloc_traits =
        typename alloc_traits::template rebind_traits<char>;
    using char_alloc_t = typename char_alloc_traits::allocator_type;

    char_alloc_t alloc{alloc_};
    void* p = char_alloc_traits::allocate(alloc, sizeof(node_base) + sizeof(T));
    new (p) node_base(node_traits<T>::kind);
    char* by_p = static_cast<char*>(p);
    by_p += sizeof(node_base);
    new (static_cast<void*>(by_p)) T(static_cast<Args&&>(args)...);
    return node_ptr<T>{static_cast<node_base*>(p)};
}
} // namespace ely