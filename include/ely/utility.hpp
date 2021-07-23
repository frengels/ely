#pragma once

#include "ely/defines.h"

#include <memory>
#include <optional>
#include <type_traits>

namespace ely
{
template<typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, typename... Us>
inline constexpr bool is_same_one_of_v = (std::is_same_v<T, Us> || ...);

template<typename... Ts>
struct type_list
{
    template<template<typename...> class T>
    using apply_all = T<Ts...>;

    template<template<typename> class T>
    using apply_each = type_list<T<Ts>...>;
};

template<std::size_t I, typename... Ts>
using nth_element_t = std::tuple_element_t<I, std::tuple<Ts...>>;

template<typename... Ts>
struct first_element
{};

template<typename T, typename... Rs>
struct first_element<T, Rs...>
{
    using type = T;
};

template<typename... Ts>
using first_element_t = typename first_element<Ts...>::type;

namespace detail
{
struct type_index_helper
{
    constexpr std::size_t
    operator()(std::initializer_list<bool> sames) const noexcept
    {
        std::size_t index = 0;

        for (auto same : sames)
        {
            if (same)
            {
                return index;
            }

            ++index;
        }

        return index;
    }
};
} // namespace detail

template<typename T, typename... Ts>
inline constexpr std::size_t
    type_index_v = detail::type_index_helper{}({std::is_same_v<T, Ts>...});

template<typename T, typename... Ts>
struct type_index : std::integral_constant<std::size_t, type_index_v<T, Ts...>>
{};

// there's no added template instantiation overhead since all the is_same_v
// would get instantiated anyway
template<typename T, typename... Ts>
inline constexpr std::size_t type_present_v = type_index_v<T, Ts...> !=
                                              sizeof...(Ts);

template<typename T, typename... Ts>
struct type_present
    : std::integral_constant<std::size_t, type_present_v<T, Ts...>>
{};

namespace detail
{
struct type_count_helper
{
    constexpr std::size_t
    operator()(std::initializer_list<bool> sames) const noexcept
    {
        std::size_t count = 0;

        for (auto same : sames)
        {
            if (same)
            {
                ++count;
            }
        }

        return count;
    }
};
} // namespace detail

template<typename T, typename... Ts>
inline constexpr std::size_t
    type_count_v = detail::type_count_helper{}({std::is_same_v<T, Ts>...});

template<typename T, typename... Ts>
struct type_count : std::integral_constant<std::size_t, type_count_v<T, Ts...>>
{};

/// counts how often a type is repeated in a pack

namespace detail
{

struct type_unique_helper
{
    constexpr bool
    operator()(std::initializer_list<bool> sameness) const noexcept
    {
        bool found = false;

        for (auto same : sameness)
        {
            if (same)
            {
                if (found)
                {
                    return false;
                }

                found = true;
            }
        }

        return found;
    }
};

} // namespace detail

template<typename T, typename... Ts>
inline constexpr std::size_t
    type_unique_v = detail::type_unique_helper{}({std::is_same_v<T, Ts>...});

template<typename T, typename... Ts>
struct type_unique : std::bool_constant<type_unique_v<T, Ts...>>
{};

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

template<typename T>
ELY_ALWAYS_INLINE void* voidify(T& ref) noexcept
{
    return const_cast<void*>(
        static_cast<const volatile void*>(std::addressof(ref)));
}

template<typename Alloc,
         typename InputIt,
         typename Sentinel,
         typename ForwardIt>
ELY_ALWAYS_INLINE ForwardIt uninitialized_move_a(Alloc&    alloc,
                                                 InputIt   first,
                                                 Sentinel  last,
                                                 ForwardIt dst_first)
{
    for (; first != last; ++first, ++dst_first)
    {
        std::allocator_traits<Alloc>::construct(
            alloc, std::addressof(*dst_first), std::move(*first));
    }

    return dst_first;
}

template<typename Alloc,
         typename InputIt,
         typename Sentinel,
         typename ForwardIt>
ELY_ALWAYS_INLINE ForwardIt uninitialized_copy_a(Alloc&    alloc,
                                                 InputIt   first,
                                                 Sentinel  last,
                                                 ForwardIt dst_first)
{
    for (; first != last; ++first, ++dst_first)
    {
        std::allocator_traits<Alloc>::construct(
            alloc, std::addressof(*dst_first), *first);
    }

    return dst_first;
}
} // namespace ely