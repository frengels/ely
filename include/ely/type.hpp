#pragma once

#ifndef ELY_TYPE_HPP
#define ELY_TYPE_HPP

#include <type_traits>

#include <cstdint>

#include "ely/type.h"

namespace ely
{
constexpr ely_type get_type_poison()
{
    return {ELY_TYPE_POISON, {}};
}

constexpr ely_type get_type_generic()
{
    return {ELY_TYPE_GENERIC, {}};
}

constexpr ely_type get_type_void()
{
    return {ELY_TYPE_VOID, {}};
}

constexpr ely_type get_type_u32()
{
    return {ELY_TYPE_U32, {}};
}

constexpr ely_type get_type_u64()
{
    return {ELY_TYPE_U64, {}};
}

constexpr ely_type get_type_s32()
{
    return {ELY_TYPE_S32, {}};
}

constexpr ely_type get_type_s64()
{
    return {ELY_TYPE_S64, {}};
}

constexpr ely_type get_type_f32()
{
    return {ELY_TYPE_F32, {}};
}

constexpr ely_type get_type_f64()
{
    return {ELY_TYPE_F64, {}};
}

template<typename T>
constexpr ely_type get_type()
{
    if constexpr (std::is_same_v<void, T>)
    {
        return get_type_void();
    }
    else if constexpr (std::is_same_v<std::uint32_t, T>)
    {
        return get_type_u32();
    }
    else if constexpr (std::is_same_v<std::uint64_t, T>)
    {
        return get_type_u64();
    }
    else if constexpr (std::is_same_v<std::int32_t, T>)
    {
        return get_type_s32();
    }
    else if constexpr (std::is_same_v<std::int64_t, T>)
    {
        return get_type_s64();
    }
    else if constexpr (std::is_same_v<float, T>)
    {
        return get_type_f32();
    }
    else if constexpr (std::is_same_v<double, T>)
    {
        return get_type_f64();
    }
    else
    {
        static_assert(!std::is_same_v<int, int>,
                      "cannot get ely type for c++ type");
        return get_type_poison();
    }
}

template<typename T>
constexpr ely_type type_of(T val)
{
    return ely::get_type<T>();
}
} // namespace ely

#endif