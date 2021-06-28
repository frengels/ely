#pragma once

#include "ely/defines.h"

#include <memory>
#include <type_traits>

namespace ely
{
template<typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

namespace detail
{
template<typename P, typename = void>
struct to_address_helper;
}

template<typename T>
ELY_ALWAYS_INLINE constexpr T* to_address(T* p) noexcept
{
    static_assert(!std::is_function_v<T>, "T is a function type");
    return p;
}

template<typename P, typename = std::enable_if_t<!std::is_pointer_v<P>>>
ELY_ALWAYS_INLINE constexpr auto to_address(const P& p) noexcept
    -> decltype(detail::to_address_helper<P>::call(p))
{
    return detail::to_address_helper<P>::call(p);
}

namespace detail
{
template<typename P, typename>
struct to_address_helper
{
    ELY_ALWAYS_INLINE static constexpr auto call(const P& p) noexcept
        -> decltype(ely::to_address(p.operator->()))
    {
        return ely::to_address(p.operator->());
    }
};

template<typename P>
struct to_address_helper<
    P,
    std::void_t<decltype(std::pointer_traits<P>::to_address(
        std::declval<const P&>()))>>
{
    ELY_ALWAYS_INLINE static constexpr auto call(const P& p) noexcept
        -> decltype(std::pointer_traits<P>::to_address(p))
    {
        return std::pointer_traits<P>::to_address(p);
    }
};
} // namespace detail
} // namespace ely