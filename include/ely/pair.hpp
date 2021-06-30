#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

#include "ely/defines.h"

namespace ely
{
namespace detail
{
/// A box which can be tagged to prevent nasty inheritance issues
template<int I, typename T, bool Empty = std::is_empty_v<T>>
class PairBox;

template<int I, typename T>
class PairBox<I, T, true> : private T
{
public:
    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Args&&... args)
        : T(static_cast<Args&&>(args)...)
    {}

    template<typename Tuple, std::size_t... Is>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Tuple&& t,
                                                 std::index_sequence<Is...>)
        : T(std::get<Is>(static_cast<Tuple&&>(t))...)
    {}

    ELY_ALWAYS_INLINE constexpr const T& get() const& noexcept
    {
        return static_cast<T&>(*this);
    }
};

template<int I, typename T>
class PairBox<I, T, false>
{
    T value;

public:
    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Args&&... args)
        : value(static_cast<Args&&>(args)...)
    {}

    template<typename Tuple, std::size_t... Is>
    ELY_ALWAYS_INLINE explicit constexpr PairBox(Tuple&& t,
                                                 std::index_sequence<Is...>)
        : value(std::get<Is>(static_cast<Tuple&&>(t))...)
    {}

    ELY_ALWAYS_INLINE constexpr const T& get() const& noexcept
    {
        return value;
    }
};
} // namespace detail

/// pair using EBO (empty base optimization) to use up no extra space for empty
/// types
/// Very simple implementation for now
template<typename T1, typename T2>
class EBOPair : private detail::PairBox<0, T1>, private detail::PairBox<1, T2>
{
private:
    using first_box  = detail::PairBox<0, T1>;
    using second_box = detail::PairBox<1, T2>;

public:
    using first_type  = T1;
    using second_type = T2;

private:
    ELY_ALWAYS_INLINE constexpr const first_type& cfirst() const noexcept
    {
        return static_cast<const detail::PairBox<0, first_type>&>(*this).get();
    }

    ELY_ALWAYS_INLINE constexpr const second_type& csecond() const noexcept
    {
        return static_cast<const detail::PairBox<1, second_type>&>(*this).get();
    }

public:
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

    ELY_ALWAYS_INLINE constexpr const first_type& first() const& noexcept
    {
        return cfirst();
    }

    ELY_ALWAYS_INLINE constexpr first_type& first() & noexcept
    {
        return const_cast<first_type&>(cfirst());
    }

    ELY_ALWAYS_INLINE constexpr const first_type&& first() const&& noexcept
    {
        return std::move(cfirst());
    }

    ELY_ALWAYS_INLINE constexpr first_type&& first() && noexcept
    {
        return std::move(first());
    }

    ELY_ALWAYS_INLINE constexpr const second_type& second() const& noexcept
    {
        return csecond();
    }

    ELY_ALWAYS_INLINE constexpr second_type& second() & noexcept
    {
        return const_cast<second_type&>(csecond());
    }

    ELY_ALWAYS_INLINE constexpr second_type&& second() && noexcept
    {
        return std::move(first());
    }

    ELY_ALWAYS_INLINE constexpr const second_type&& second() const&& noexcept
    {
        return std::move(cfirst());
    }
};

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 0, T1&> get(EBOPair<T1, T2>& p) noexcept
{
    return p.first();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 0, const T1&>
get(const EBOPair<T1, T2>& p) noexcept
{
    return p.first();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 0, T1&&> get(EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).first();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 0, const T1&&>
get(const EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).first();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 1, T2&> get(EBOPair<T1, T2>& p) noexcept
{
    return p.second();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 1, const T2&>
get(const EBOPair<T1, T2>& p) noexcept
{
    return p.second();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 1, T2&&> get(EBOPair<T1, T2>&& p) noexcept
{
    return std::move(p).second();
}

template<std::size_t I, typename T1, typename T2>
constexpr std::enable_if_t<I == 1, const T2&&>
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