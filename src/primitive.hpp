#pragma once

#ifndef PRIMITIVE_HPP
#define PRIMITIVE_HPP

#include <cassert>
#include <cstdint>

#include "ely/node.h"
#include "ely/type.h"

struct ely_value;

using ely_fn_ptr_t = ely_value* (*) (ely_value**);

struct primitive_info
{
    ely_fn_ptr_t    fn;
    ely_type        res;
    const ely_type* args;
    std::size_t     args_len;

    constexpr bool matches(ely_type        ret_ty,
                           const ely_type* args_ty,
                           std::size_t     args_len) const
    {
        if (this->args_len != args_len)
        {
            return false;
        }

        if (!ely_type_match(&res, &ret_ty))
        {
            return false;
        }

        const ely_type* our_it = args;
        const ely_type* it     = args_ty;
        const ely_type* end    = args_ty + args_len;
        for (; it != end; ++it, ++our_it)
        {
            if (!ely_type_match(our_it, it))
            {
                return false;
            }
        }

        return true;
    }
};

struct primitive_overload_set
{
    using iterator = const primitive_info*;

    const primitive_info* infos;
    const std::size_t     infos_len;

    constexpr iterator begin() const
    {
        return infos;
    }

    constexpr iterator end() const
    {
        return infos + infos_len;
    }
};

extern const std::size_t             primitives_len;
extern const primitive_overload_set* primitives;

ely_fn_ptr_t select_primitive_overload(ely_prim_kind   kind,
                                       ely_type        ret_ty,
                                       const ely_type* args_ty,
                                       std::size_t     args_len);

#endif