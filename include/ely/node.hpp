#pragma once

#include <cstdint>

#include "ely/ilist.hpp"

namespace ely
{
enum struct node_kind : std::uint8_t
{
    stx_id,
    stx_prim_id,

    stx_list,

    stx_lit_int,
    stx_lit_dec,

    stx_lit_i32,
    stx_lit_i64,
    stx_lit_u32,
    stx_lit_u64,
    stx_lit_f32,
    stx_lit_f64,

    call_user,
    call_prim,
};

struct node_base : private ely::ilink
{
    friend ely::ilink_access;

    node_kind kind;
};
} // namespace ely