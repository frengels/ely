#pragma once

#include <functional>
#include <initializer_list>
#include <limits>
#include <tuple>
#include <variant>

#include "ely/defines.h"
#include "ely/union.hpp"

namespace ely
{
template<typename V>
struct variant_size;

template<typename V>
static constexpr std::size_t variant_size_v = variant_size<V>::value;

namespace variant
{
constexpr bool check_any_false(std::initializer_list<bool> blist) noexcept
{
    for (bool b : blist)
    {
        if (!b)
        {
            return false;
        }
    }

    return true;
}

template<std::size_t I, typename... Ts>
using nth_element_t = std::tuple_element_t<I, std::tuple<Ts...>>;

template<typename T, std::size_t I>
struct OverloadResult
{
    using type                         = T;
    static constexpr std::size_t value = I;
};

template<std::size_t I, typename...>
struct OverloadImpl;

template<std::size_t I>
struct OverloadImpl<I>
{
    void operator()() const;
};

template<std::size_t I, typename T, typename... Ts>
struct OverloadImpl<I, T, Ts...>
{
    using OverloadImpl<I + 1, Ts...>::operator();
    OverloadResult<T, I>              operator()(T) const;
};

template<typename... Ts>
struct Overload
{
    using OverloadImpl<0, Ts...>::operator();
};

template<typename U, typename... Ts>
using ResolveOverloadType =
    typename decltype(Overload<Ts...>()(std::declval<U>()))::type;

template<typename U, typename... Ts>
static constexpr std::size_t ResolveOverloadIndex =
    decltype(Overload<Ts...>()(std::declval<U>()))::value;

template<bool Valid, typename R>
struct Dispatcher;

template<typename R>
struct Dispatcher<false, R>
{
    template<std::size_t I, typename V, typename F>
    ELY_ALWAYS_INLINE static constexpr R case_(V&&, F&&)
    {
        __builtin_unreachable();
    }

    template<std::size_t B, typename V, typename F>
    ELY_ALWAYS_INLINE static constexpr R switch_(V&&, F&&)
    {
        __builtin_unreachable();
    }
};

template<typename R>
struct Dispatcher<true, R>
{
    template<std::size_t I, typename V, typename F>
    ELY_ALWAYS_INLINE static constexpr R case_(V&& fn, F&& v)
    {
        using Expected = R;
        using Actual   = decltype(std::invoke(
            static_cast<F&&>(fn),
            static_cast<V&&>(v).template get_unchecked<I>()));
        static_assert(std::is_same_v<Expected, Actual>,
                      "visit requires a single return type");
        return std::invoke(static_cast<F&&>(fn),
                           static_cast<V&&>(v).template get_unchecked<I>());
    }

    template<std::size_t B, typename V, typename F>
    ELY_ALWAYS_INLINE static constexpr R switch_(V&& fn, F&& v)
    {
        constexpr auto size = std::variant_size_v<std::remove_cvref_t<V>>;

#define DISPATCH(idx)                                                          \
    case B + idx:                                                              \
        return Dispatcher<(B + idx) < size, R>::template case_<B + idx>(       \
            static_cast<F&&>(fn), static_cast<V&&>(v))

        switch (v.index())
        {
            DISPATCH(0);
            DISPATCH(1);
            DISPATCH(2);
            DISPATCH(3);
            DISPATCH(4);
            DISPATCH(5);
            DISPATCH(6);
            DISPATCH(7);
#undef DISPATCH
        default:
            return Dispatcher<(B + 8) < size, R>::template switch_<B + 8>(
                static_cast<F&&>(fn), static_cast<V&&>(v));
        }
    }
};

class NoIndex
{
public:
    NoIndex() = default;

    constexpr operator std::size_t() const noexcept
    {
        return 0;
    }
};
template<std::size_t N>
using variant_index_t = std::conditional_t<
    N == 0,
    NoIndex,
    std::conditional_t<
        N <= std::numeric_limits<uint8_t>::max(),
        uint8_t,
        std::conditional_t<
            N <= std::numeric_limits<uint16_t>::max(),
            uint16_t,
            std::conditional_t<N <= std::numeric_limits<uint32_t>::max(),
                               uint32_t,
                               uint64_t>>>>;

template<typename... Ts>
class Variant
{
private:
    using union_type = ely::Union<Ts...>;

public:
    using index_type = variant_index_t<sizeof...(Ts)>;

private:
    [[no_unique_address]] union_type union_{};
    [[no_unique_address]] index_type index_{};

public:
    Variant() = default;

    constexpr std::size_t index() const
    {
        return index_;
    }

    template<std::size_t I>
    constexpr decltype(auto) get_unchecked() & noexcept
    {
        return union_.template get_unchecked<I>();
    }

    template<std::size_t I>
    constexpr decltype(auto) get_unchecked() const& noexcept
    {
        return union_.template get_unchecked<I>();
    }

    template<std::size_t I>
    constexpr decltype(auto) get_unchecked() && noexcept
    {
        return static_cast<union_type&&>(union_).template get_unchecked<I>();
    }

    template<std::size_t I>
    constexpr decltype(auto) get_unchecked() const&& noexcept
    {
        return static_cast<const union_type&&>(union_)
            .template get_unchecked<I>();
    }

    template<typename V, typename F>
    friend constexpr decltype(auto) visit(V&& v, F&& fn)
    {
        using R = decltype(std::invoke(
            static_cast<F&&>(fn),
            static_cast<V&&>(v).template get_unchecked<0>()));
        return Dispatcher<true, R>::template switch_(static_cast<V&&>(v),
                                                     static_cast<F&&>(fn));
    }
};
} // namespace variant

template<typename... Ts>
using Variant = std::variant<Ts...>;

template<typename F, typename... Ts>
constexpr auto visit(Variant<Ts...>& var, F&& fn) -> decltype(auto)
{
    return std::visit(static_cast<F&&>(fn), var);
}

template<typename F, typename... Ts>
constexpr auto visit(const Variant<Ts...>& var, F&& fn) -> decltype(auto)
{
    return std::visit(static_cast<F&&>(fn), var);
}

template<typename F, typename... Ts>
constexpr auto visit(Variant<Ts...>&& var, F&& fn) -> decltype(auto)
{
    return std::visit(static_cast<F&&>(fn), std::move(var));
}

template<typename F, typename... Ts>
constexpr auto visit(const Variant<Ts...>&& var, F&& fn) -> decltype(auto)
{
    return std::visit(static_cast<F&&>(fn), std::move(var));
}
} // namespace ely

namespace std
{
template<typename... Ts>
struct variant_size<ely::variant::Variant<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};
} // namespace std