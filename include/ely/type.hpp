#pragma once

#ifndef ELY_TYPE_HPP
#define ELY_TYPE_HPP

#include <type_traits>

#include <cstdint>

#include "ely/type.h"

namespace ely
{
template<typename T>
constexpr ely_type get_type()
{
    if constexpr (std::is_same_v<void, T>)
    {
        return {ELY_TYPE_VOID, {}};
    }
    if constexpr (std::is_same_v<std::uint32_t, T>)
    {
        return {ELY_TYPE_U32, {}};
    }
    else if constexpr (std::is_same_v<std::uint64_t, T>)
    {
        return {ELY_TYPE_U64, {}};
    }
    else if constexpr (std::is_same_v<std::int32_t, T>)
    {
        return {ELY_TYPE_S32, {}};
    }
    else if constexpr (std::is_same_v<std::int64_t, T>)
    {
        return {ELY_TYPE_S64, {}};
    }
    else if constexpr (std::is_same_v<float, T>)
    {
        return {ELY_TYPE_F32, {}};
    }
    else if constexpr (std::is_same_v<double, T>)
    {
        return {ELY_TYPE_F64, {}};
    }
    else
    {
        static_assert(!std::is_same_v<int, int>,
                      "cannot get ely type for c++ type");
        return {ELY_TYPE_POISON, {}};
    }
}

template<typename T>
constexpr ely_type type_of(T val)
{
    return ely::get_type<T>();
}
} // namespace ely

#endif