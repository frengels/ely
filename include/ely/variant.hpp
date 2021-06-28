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

template<typename T, typename... Ts>
static constexpr std::size_t FindElementIndex = [] {
    constexpr bool v[] = {std::is_same_v<T, Ts>...};
    std::size_t    idx = 0;

    auto first = std::begin(v);
    auto last  = std::end(v);

    while (first != last && !*first)
    {
        ++idx;
        ++first;
    }

    return idx;
}();

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
struct OverloadImpl<I, T, Ts...> : OverloadImpl<I + 1, Ts...>
{
    using OverloadImpl<I + 1, Ts...>::operator();
    OverloadResult<T, I>              operator()(T) const;
};

template<typename... Ts>
struct Overload : OverloadImpl<0, Ts...>
{
    using OverloadImpl<0, Ts...>::operator();
};

template<typename U, typename... Ts>
using ResolveOverloadType =
    typename decltype(Overload<Ts...>()(std::declval<U>()))::type;

template<typename U, typename... Ts>
static constexpr std::size_t ResolveOverloadIndex =
    decltype(Overload<Ts...>()(std::declval<U>()))::value;

template<bool Valid, typename R, std::size_t N>
struct Dispatcher;

template<typename R, std::size_t N>
struct Dispatcher<false, R, N>
{
    template<std::size_t I, typename F>
    ELY_ALWAYS_INLINE static constexpr R case_(F&&)
    {
        __builtin_unreachable();
    }

    template<std::size_t B, typename F>
    ELY_ALWAYS_INLINE static constexpr R switch_(F&&, std::size_t)
    {
        __builtin_unreachable();
    }
};

template<typename R, std::size_t N>
struct Dispatcher<true, R, N>
{
    template<std::size_t I, typename F>
    ELY_ALWAYS_INLINE static constexpr R case_(F&& fn)
    {
        using Expected = R;
        using Actual   = decltype(std::invoke(
            static_cast<F&&>(fn), std::integral_constant<std::size_t, I>{}));

        static_assert(std::is_same_v<Expected, Actual>,
                      "visit requires a single return type");

        return std::invoke(static_cast<F&&>(fn),
                           std::integral_constant<std::size_t, I>{});
    }

    template<std::size_t B, typename F>
    ELY_ALWAYS_INLINE static constexpr R switch_(F&& fn, std::size_t index)
    {
#define DISPATCH(idx)                                                          \
    case B + idx:                                                              \
        return Dispatcher<(B + idx) < N, R, N>::template case_<B + idx>(       \
            static_cast<F&&>(fn))

        switch (index)
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
            return Dispatcher<(B + 8) < N, R, N>::template switch_<B + 8>(
                static_cast<F&&>(fn), index);
        }
    }
};

template<std::size_t N, typename F>
ELY_ALWAYS_INLINE constexpr auto dispatch_index(F&& fn, std::size_t index)
    -> decltype(std::invoke(
        static_cast<F&&>(fn),
        std::integral_constant<std::size_t, 0>{})) requires(N >= 1)
{
    using R = decltype(std::invoke(static_cast<F&&>(fn),
                                   std::integral_constant<std::size_t, 0>{}));
    return Dispatcher<true, R, N>::template switch_<0>(static_cast<F&&>(fn),
                                                       index);
}

class NoIndex
{
public:
    NoIndex() = default;

    // this is to make construction from std::size_t easier
    constexpr NoIndex(std::size_t)
    {}

    constexpr operator std::size_t() const noexcept
    {
        return 0;
    }
};
template<std::size_t N>
using variant_index_t = std::conditional_t<
    N <= 1,
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
class Variant2;

template<ely::detail::Availability A, typename... Ts>
class VariantDestructor;

#define VARIANT_IMPL(availability, destructor)                                 \
    template<typename... Ts>                                                   \
    class VariantDestructor<availability, Ts...>                               \
    {                                                                          \
        friend struct Access;                                                  \
                                                                               \
        [[no_unique_address]] ely::Union<Ts...> union_{                        \
            std::in_place_index<0>};                                           \
        [[no_unique_address]] variant_index_t<sizeof...(Ts)> index_{0};        \
                                                                               \
    public:                                                                    \
        VariantDestructor() = default;                                         \
                                                                               \
        template<std::size_t I, typename... Args>                              \
        explicit constexpr VariantDestructor(std::in_place_index_t<I>,         \
                                             Args&&... args)                   \
            : union_(std::in_place_index<I>, static_cast<Args&&>(args)...),    \
              index_(I)                                                        \
        {}                                                                     \
                                                                               \
        VariantDestructor(const VariantDestructor&) = default;                 \
        VariantDestructor(VariantDestructor&&)      = default;                 \
                                                                               \
        destructor         VariantDestructor&                                  \
                           operator=(const VariantDestructor&) = default;      \
        VariantDestructor& operator=(VariantDestructor&&) = default;           \
                                                                               \
        constexpr std::size_t index() const noexcept                           \
        {                                                                      \
            return static_cast<std::size_t>(index_);                           \
        }                                                                      \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             ~VariantDestructor() = default;);
VARIANT_IMPL(
    ely::detail::Availability::Available,
    ~VariantDestructor() {
        dispatch_index<sizeof...(Ts)>([&](auto i) noexcept {
            constexpr auto I = decltype(i)::value;
            destroy<I>(this->union_);
        });
    });
VARIANT_IMPL(ely::detail::Availability::Unavailable,
             ~VariantDestructor() = delete;);

#undef VARIANT_IMPL

template<typename... Ts>
class Variant2 : public VariantDestructor<
                     ely::detail::CommonAvailability<Ts...>::destructible,
                     Ts...>
{
    using base_ =
        VariantDestructor<ely::detail::CommonAvailability<Ts...>::destructible,
                          Ts...>;

public:
    using base_::base_;
    using base_::index;
};

template<typename... Ts>
class Variant;

namespace detail
{
template<typename T>
struct is_variant : std::false_type
{};

template<typename... Ts>
struct is_variant<Variant<Ts...>> : std::true_type
{};

template<typename... Ts>
struct is_variant<Variant2<Ts...>> : std::true_type
{};
} // namespace detail

template<typename... Ts>
class Variant
{
    friend struct Access;

private:
    using union_type = ely::Union<Ts...>;

public:
    using index_type = variant_index_t<sizeof...(Ts)>;

private:
    [[no_unique_address]] union_type union_{};
    [[no_unique_address]] index_type index_{};

public:
    Variant() = default;

    template<typename U>
    constexpr Variant(U&& u) requires(
        !std::same_as<Variant<Ts...>, std::remove_cvref_t<U>>)
        : union_(std::in_place_index<ResolveOverloadIndex<U, Ts...>>,
                 static_cast<U&&>(u)),
          index_(ResolveOverloadIndex<U, Ts...>)
    {}

    template<std::size_t I, typename... Args>
    explicit constexpr Variant(std::in_place_index_t<I> idx, Args&&... args)
        : union_(idx, static_cast<Args&&>(args)...), index_(I)
    {}

    template<typename T, typename... Args>
    explicit constexpr Variant(std::in_place_type_t<T>, Args&&... args)
        : union_(std::in_place_index<FindElementIndex<T, Ts...>>,
                 static_cast<Args&&>(args)...),
          index_(static_cast<index_type>(FindElementIndex<T, Ts...>))
    {}

    Variant(const Variant&) requires(
        (std::is_copy_constructible_v<Ts> && ...) &&
        (std::is_trivially_copy_constructible_v<Ts> && ...)) = default;
    Variant(const Variant&) requires(!(std::is_copy_constructible_v<Ts> &&
                                       ...))                 = delete;
    constexpr Variant(const Variant& other) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> &&
         ...)) requires((std::is_copy_constructible_v<Ts> && ...) &&
                        !(std::is_trivially_copy_constructible_v<Ts> && ...))
        : union_(dispatch_index<sizeof...(Ts)>(
              [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                  return union_type(std::in_place_index<I>,
                                    get_unchecked<I>(other.union_));
              },
              other.index())),
          index_(other.index_)
    {}

    Variant(Variant&&) requires((std::is_move_constructible_v<Ts> && ...) &&
                                (std::is_trivially_move_constructible_v<Ts> &&
                                 ...))  = default;
    Variant(Variant&&) requires(!(std::is_move_constructible_v<Ts> &&
                                  ...)) = delete;
    constexpr Variant(Variant&& other) noexcept(
        (std::is_nothrow_move_constructible_v<Ts> &&
         ...)) requires((std::is_move_constructible_v<Ts> && ...) &&
                        !(std::is_trivially_move_constructible_v<Ts> && ...))
        : union_(dispatch_index<sizeof...(Ts)>(
              [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                  return union_type(
                      std::in_place_index<I>,
                      get_unchecked<I>((std::move(other).union_)));
              },
              other.index())),
          index_(other.index_)
    {}

    Variant& operator=(const Variant&) requires(
        (std::is_copy_assignable_v<Ts> && ...) &&
        (std::is_trivially_copy_assignable_v<Ts> && ...)) = default;
    Variant& operator                            =(const Variant&) requires(
        !(std::is_copy_assignable_v<Ts> && ...)) = delete;
    constexpr Variant& operator=(const Variant& other) noexcept(
        (std::is_nothrow_copy_assignable_v<Ts> &&
         ...)) requires((std::is_copy_assignable_v<Ts> && ...) &&
                        !(std::is_trivially_copy_assignable_v<Ts> && ...))
    {
        dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                destroy<I>(union_);
            },
            index());

        index_ = other.index_;
        dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                emplace<I>(union_, get_unchecked<I>(other.union_));
            },
            index());

        return *this;
    }

    Variant& operator                                     =(Variant&&) requires(
        (std::is_move_assignable_v<Ts> && ...) &&
        (std::is_trivially_move_assignable_v<Ts> && ...)) = default;
    Variant& operator=(Variant&&) requires(!(std::is_move_assignable_v<Ts> &&
                                             ...)) = delete;
    constexpr Variant& operator                    =(Variant&& other) noexcept(
        (std::is_nothrow_move_assignable_v<Ts> &&
         ...)) requires((std::is_move_assignable_v<Ts> && ...) &&
                        !(std::is_trivially_move_assignable_v<Ts> && ...))
    {
        dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                destroy<I>(union_);
            },
            index());

        index_ = other.index_;

        dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                emplace<I>(union_, get_unchecked<I>((std::move(other).union_)));
            },
            index());

        return *this;
    }

    ~Variant() requires((std::is_destructible_v<Ts> && ...) &&
                        (std::is_trivially_destructible_v<Ts> &&
                         ...))                                = default;
    ~Variant() requires(!(std::is_destructible_v<Ts> && ...)) = delete;
    ~Variant() requires((std::is_destructible_v<Ts> && ...) &&
                        !(std::is_trivially_destructible_v<Ts> && ...))
    {
        dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) {
                destroy<I>(union_);
            },
            index());
    }

    constexpr std::size_t index() const noexcept
    {
        return static_cast<std::size_t>(index_);
    }


    template<typename T>
    friend constexpr bool holds_alternative(const Variant& v) noexcept
    {
        return dispatch_index<sizeof...(Ts)>(
            []<std::size_t I>(std::integral_constant<std::size_t, I>) -> bool {
                return std::is_same_v<nth_element_t<I, Ts...>, T>;
            },
            v.index());
    }

    template<typename T>
    friend constexpr bool holds(const Variant& v) noexcept
    {
        return holds_alternative<sizeof...(Ts)>(v);
    }

    template<std::size_t I>
    friend constexpr bool holds_alternative(const Variant& v) noexcept
    {
        return dispatch_index<sizeof...(Ts)>(
            []<std::size_t J>(std::integral_constant<std::size_t, J>) -> bool {
                return I == J;
            },
            v.index());
    }

    template<std::size_t I>
    friend constexpr bool holds(const Variant& v) noexcept
    {
        return holds_alternative<I>(v);
    }

    template<typename F,
             typename R = std::invoke_result_t<
                 F,
                 decltype(get_unchecked<0>((std::declval<Variant&>().union_)))>>
    constexpr R visit(F&& fn) &
    {
        return dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) -> R {
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(this->union_));
            },
            index());
    }

    template<
        typename F,
        typename R = std::invoke_result_t<
            F,
            decltype(get_unchecked<0>(std::declval<const Variant&>().union_))>>
    constexpr R visit(F&& fn) const&
    {
        return dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) -> R {
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(this->union_));
            },
            index());
    }

    template<
        typename F,
        typename R = std::invoke_result_t<
            F,
            decltype(get_unchecked<0>((std::declval<Variant&&>().union_)))>>
    constexpr R visit(F&& fn) &&
    {
        return dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) -> R {
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>((std::move(*this).union_)));
            },
            index());
    }

    template<typename F,
             typename R = std::invoke_result_t<
                 F,
                 decltype(get_unchecked<0>(
                     (std::declval<const Variant&&>().union_)))>>
    constexpr R visit(F&& fn) const&&
    {
        return dispatch_index<sizeof...(Ts)>(
            [&]<std::size_t I>(std::integral_constant<std::size_t, I>) -> R {
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>((std::move(*this).union_)));
            },
            index());
    }
};

struct Access
{
    template<std::size_t I, typename V>
    static constexpr auto&& get_unchecked(V&& v) noexcept
    {
        return ely::get_unchecked<I>((static_cast<V&&>(v).union_));
    }
};

template<std::size_t I,
         typename V,
         typename = std::enable_if_t<
             detail::is_variant<std::remove_cvref_t<V>>::value>>
constexpr auto&& get_unchecked(V&& v) noexcept
{
    return Access::get_unchecked<I>(static_cast<V&&>(v));
}

template<
    typename V,
    typename F,
    typename R =
        std::invoke_result_t<F, decltype(get_unchecked<0>(std::declval<V>()))>>
constexpr R
visit(V&& v, F&& fn) requires(detail::is_variant<std::remove_cvref_t<V>>::value)
{
    return dispatch_index<std::variant_size_v<std::remove_cvref_t<V>>>(
        [&]<std::size_t I>(std::integral_constant<std::size_t, I>) -> R {
            return std::invoke(static_cast<F&&>(fn),
                               get_unchecked<I>(static_cast<V&&>(v)));
        },
        v.index());
}
} // namespace variant

template<typename... Ts>
using Variant = variant::Variant2<Ts...>;

template<typename V, typename F>
constexpr decltype(auto) visit(V&& v, F&& fn)
{
    return ely::variant::visit(static_cast<V&&>(v), static_cast<F&&>(fn));
}
} // namespace ely

namespace std
{
template<typename... Ts>
struct variant_size<ely::variant::Variant<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};
} // namespace std