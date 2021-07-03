#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include "ely/defines.h"

namespace ely
{
namespace detail
{
/// This box enables the use of EBO, the idea is when a type is not empty it
/// will not be used as a base and when a type is empty it can be the base of
/// this Box.
/// In the case that both types T1 and T2 are equal and they are an empty type
/// thus suitable for performing EBO, the second box will not inherit or contain
/// any type, therefore both first() and second() sharing the same location in
/// memory.
template<int I,
         typename T1,
         typename T2,
         bool Empty = std::is_empty_v<std::conditional_t<I == 0, T1, T2>>>
class PairBox;

template<int I, typename T1, typename T2>
class PairBox<I, T1, T2, true> : private std::conditional_t<I == 0, T1, T2>
{
private:
    using base_ = std::conditional_t<I == 0, T1, T2>;

    using value_type = base_;

public:
    PairBox() = default;

    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Args&&... args)
        : base_(static_cast<Args&&>(args)...)
    {}

    template<typename Tuple, std::size_t... Is>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Tuple&& t,
                                                 std::index_sequence<Is...>)
        : base_(std::get<Is>(static_cast<Tuple&&>(t))...)
    {}

    ELY_ALWAYS_INLINE constexpr value_type& get() & noexcept
    {
        return static_cast<value_type&>(*this);
    }

    ELY_ALWAYS_INLINE constexpr const value_type& get() const& noexcept
    {
        return static_cast<const value_type&>(*this);
    }

    ELY_ALWAYS_INLINE constexpr value_type&& get() && noexcept
    {
        return static_cast<value_type&&>(*this);
    }

    ELY_ALWAYS_INLINE constexpr const value_type&& get() const&& noexcept
    {
        return static_cast<const value_type&&>(*this);
    }
};

template<typename T>
class PairBox<1, T, T, true>
{
public:
    PairBox() = default;

    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        T(static_cast<Args&&>(args)...);
    }

    template<typename Tuple, std::size_t... Is>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Tuple&& t,
                                                 std::index_sequence<Is...>)
    {
        T(std::get<Is>(static_cast<Tuple&&>(t))...);
    }
};

template<int I, typename T1, typename T2>
class PairBox<I, T1, T2, false>
{
    using value_type = std::conditional_t<I == 0, T1, T2>;

private:
    value_type value;

public:
    PairBox() = default;

    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Args&&... args)
        : value(static_cast<Args&&>(args)...)
    {}

    template<typename Tuple, std::size_t... Is>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Tuple&& t,
                                                 std::index_sequence<Is...>)
        : value(std::get<Is>(static_cast<Tuple&&>(t))...)
    {}

    ELY_ALWAYS_INLINE constexpr value_type& get() & noexcept
    {
        return value;
    }

    ELY_ALWAYS_INLINE constexpr const value_type& get() const& noexcept
    {
        return value;
    }

    ELY_ALWAYS_INLINE constexpr value_type&& get() && noexcept
    {
        return static_cast<value_type&&>(value);
    }

    ELY_ALWAYS_INLINE constexpr const value_type&& get() const&& noexcept
    {
        return static_cast<const value_type&&>(value);
    }
};
} // namespace detail

/// pair using EBO (empty base optimization) to use up no extra space for empty
/// types
/// Very simple implementation for now
template<typename T1, typename T2>
class EBOPair : private detail::PairBox<0, T1, T2>,
                private detail::PairBox<1, T1, T2>
{
public:
    using first_type  = T1;
    using second_type = T2;

private:
    using first_box  = detail::PairBox<0, T1, T2>;
    using second_box = detail::PairBox<1, T1, T2>;

    static constexpr bool same_type_and_empty =
        std::is_same_v<T1, T2> && std::is_empty_v<T1>;

public:
    EBOPair() = default;

    ELY_ALWAYS_INLINE constexpr EBOPair(const first_type&  t1,
                                        const second_type& t2)
        : first_box(t1), second_box(t2)
    {}

    template<typename U1, typename U2>
    ELY_ALWAYS_INLINE constexpr EBOPair(U1&& u1, U2&& u2)
        : first_box(static_cast<U1&&>(u1)), second_box(static_cast<U2&&>(u2))
    {}

    template<typename... Args1, typename... Args2>
    ELY_ALWAYS_INLINE constexpr EBOPair(std::piecewise_construct_t,
                                        std::tuple<Args1...> first_args,
                                        std::tuple<Args2...> second_args)
        : first_box(std::move(first_args),
                    std::make_index_sequence<sizeof...(Args1)>{}),
          second_box(std::move(second_args),
                     std::make_index_sequence<sizeof...(Args2)>{})
    {}

    ELY_ALWAYS_INLINE constexpr first_type& first() & noexcept
    {
        return static_cast<first_box&>(*this).get();
    }

    ELY_ALWAYS_INLINE constexpr const first_type& first() const& noexcept
    {
        return static_cast<const first_box&>(*this).get();
    }

    ELY_ALWAYS_INLINE constexpr first_type&& first() && noexcept
    {
        return static_cast<first_box&&>(*this).get();
    }

    ELY_ALWAYS_INLINE constexpr const first_type&& first() const&& noexcept
    {
        return static_cast<const first_box&&>(*this).get();
    }

    ELY_ALWAYS_INLINE constexpr second_type& second() & noexcept
    {
        if constexpr (std::is_same_v<first_type, second_type> &&
                      std::is_empty_v<first_type>)
        {
            return first();
        }
        else
        {
            return static_cast<second_box&>(*this).get();
        }
    }

    ELY_ALWAYS_INLINE constexpr const second_type& second() const& noexcept
    {
        if constexpr (std::is_same_v<first_type, second_type> &&
                      std::is_empty_v<first_type>)
        {
            return first();
        }
        else
        {
            return static_cast<const second_box&>(*this).get();
        }
    }

    ELY_ALWAYS_INLINE constexpr second_type&& second() && noexcept
    {
        if constexpr (std::is_same_v<first_type, second_type> &&
                      std::is_empty_v<first_type>)
        {
            return std::move(*this).first();
        }
        else
        {
            return static_cast<second_box&&>(*this).get();
        }
    }

    ELY_ALWAYS_INLINE constexpr const second_type&& second() const&& noexcept
    {
        if constexpr (std::is_same_v<first_type, second_type> &&
                      std::is_empty_v<first_type>)
        {
            return std::move(*this).first();
        }
        else
        {
            return static_cast<const second_box&&>(*this).get();
        }
    }
};

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 0, T1&> get(EBOPair<T1, T2>& p) noexcept
{
    return p.first();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 0, const T1&>
get(const EBOPair<T1, T2>& p) noexcept
{
    return p.first();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 0, T1&&>
get(EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).first();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 0, const T1&&>
get(const EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).first();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 1, T2&>
get(EBOPair<T1, T2>& p) noexcept
{
    return p.second();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 1, const T2&>
get(const EBOPair<T1, T2>& p) noexcept
{
    return p.second();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 1, T2&&>
get(EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).second();
}

template<std::size_t I, typename T1, typename T2>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<I == 1, const T2&&>
get(const EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).second();
}
} // namespace ely

namespace std
{
template<typename T1, typename T2>
struct tuple_size<ely::EBOPair<T1, T2>> : std::integral_constant<std::size_t, 2>
{};

template<typename T1, typename T2>
struct tuple_element<0, ely::EBOPair<T1, T2>>
{
    using type = T1;
};

template<typename T1, typename T2>
struct tuple_element<1, ely::EBOPair<T1, T2>>
{
    using type = T2;
};
} // namespace std