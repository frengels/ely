#pragma once

#include <type_traits>

#include <cstdint>

#include "ely/config.h"
#include "ely/value.h"

namespace ely
{
template<typename T>
ELY_ALWAYS_INLINE ely_value create_value(T val)
{
    if constexpr (std::is_same_v<T, std::int64_t>)
    {
        return ely_value_create_s64(val);
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        return ely_value_create_s32(val);
    }
    else if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        return ely_value_create_u64(val);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return ely_value_create_u32(val);
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return ely_value_create_f64(val);
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return ely_value_create_f32(val);
    }
    else
    {
        static_assert(!std::is_same_v<int, int>, "cannot create value");
    }
}

class value
{
    ely_value impl_;

public:
    ELY_ALWAYS_INLINE value(ely_value impl) : impl_(impl)
    {}

    template<typename T>
    ELY_ALWAYS_INLINE value(T val) : impl_(create_value(val))
    {}

    

    constexpr const ely_value& impl() const
    {
        return impl_;
    }
};

template<typename T>
ELY_ALWAYS_INLINE T get_value(const value& val)
{
    if constexpr (std::is_same_v<T, std::uint64_t>)
    {
        return ely_value_get_u64(&val.impl());
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>)
    {
        return ely_value_get_u32(&val.impl());
    }
    else if constexpr (std::is_same_v<T, std::int64_t>)
    {
        return ely_value_get_s64(&val.impl());
    }
    else if constexpr (std::is_same_v<T, std::int32_t>)
    {
        return ely_value_get_s32(&val.impl());
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        return ely_value_get_f64(&val.impl());
    }
    else if constexpr (std::is_same_v<T, float>)
    {
        return ely_value_get_f32(&val.impl());
    }
    else
    {
        static_assert(!std::is_same_v<int, int>, "cannot get value");
    }
}
} // namespace ely