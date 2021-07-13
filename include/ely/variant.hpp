#pragma once

#include <functional>
#include <initializer_list>
#include <limits>
#include <tuple>
#include <variant>

#include "ely/defines.h"
#include "ely/pair.hpp"
#include "ely/union.hpp"

namespace ely
{
namespace detail
{

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
    -> decltype(std::invoke(static_cast<F&&>(fn),
                            std::integral_constant<std::size_t, 0>{}))
{
    static_assert(N >= 1,
                  "There must be at least one available number to dispatch on");
    using R = decltype(std::invoke(static_cast<F&&>(fn),
                                   std::integral_constant<std::size_t, 0>{}));
    return Dispatcher<true, R, N>::template switch_<0>(static_cast<F&&>(fn),
                                                       index);
}

template<typename R, std::size_t N, typename F>
ELY_ALWAYS_INLINE constexpr R dispatch_index(F&& fn, std::size_t index)
{
    static_assert(N >= 1,
                  "There must be atl east one available number to dispatch on");
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

    ELY_ALWAYS_INLINE constexpr operator std::size_t() const noexcept
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

} // namespace detail
template<typename... Ts>
class Variant;

namespace detail
{
// this special base should only be inherited by the variant implementation in
// this file
struct VariantBase
{};

struct VariantAccess
{
    template<std::size_t I, typename V>
    ELY_ALWAYS_INLINE static constexpr auto&& get_unchecked(V&& v) noexcept
    {
        return ely::get_unchecked<I>((static_cast<V&&>(v).get_union()));
    }

    template<typename V>
    inline static constexpr std::size_t variant_size =
        ely::remove_cvref_t<V>::variant_size_;

    template<typename V>
    inline static constexpr bool is_derived_from_variant =
        std::is_base_of_v<VariantBase, ely::remove_cvref_t<V>>;

    template<typename V>
    ELY_ALWAYS_INLINE static constexpr std::size_t index(const V& v) noexcept
    {
        return v.index();
    }
};

template<typename T>
inline constexpr bool is_derived_from_variant_v =
    VariantAccess::is_derived_from_variant<T>;
} // namespace detail

template<std::size_t I, typename V>
ELY_ALWAYS_INLINE constexpr auto
get_unchecked(V&& v) noexcept -> std::enable_if_t<
    detail::is_derived_from_variant_v<ely::remove_cvref_t<V>>,
    decltype(ely::detail::VariantAccess::get_unchecked<I>(static_cast<V&&>(v)))>
{
    return ely::detail::VariantAccess::get_unchecked<I>(static_cast<V&&>(v));
}

namespace detail
{
struct VariantUninit
{};

template<typename... Ts>
class VariantStorage : private EBOPair<ely::Union<Ts..., VariantUninit>,
                                       variant_index_t<sizeof...(Ts)>>,
                       public VariantBase
{
    static_assert((!std::is_same_v<Ts, VariantBase> && ...),
                  "Cannot store VariantBase in a Variant");
    static_assert((!std::is_same_v<Ts, VariantUninit> && ...),
                  "Cannot store VariantUninit in a Variant");

    using base_ = EBOPair<ely::Union<Ts..., VariantUninit>,
                          variant_index_t<sizeof...(Ts)>>;

protected:
    ELY_ALWAYS_INLINE constexpr VariantStorage(VariantUninit) noexcept
        : base_(std::piecewise_construct,
                std::forward_as_tuple(std::in_place_index<sizeof...(Ts)>),
                std::forward_as_tuple())
    {}

public:
    ELY_ALWAYS_INLINE constexpr VariantStorage()
        : base_(std::piecewise_construct,
                std::forward_as_tuple(std::in_place_index<0>),
                std::forward_as_tuple(variant_index_t<sizeof...(Ts)>{}))
    {}

    ELY_ALWAYS_INLINE constexpr ely::Union<Ts..., VariantUninit>&
    get_union() & noexcept
    {
        return this->first();
    }

    ELY_ALWAYS_INLINE constexpr const ely::Union<Ts..., VariantUninit>&
    get_union() const& noexcept
    {
        return this->first();
    }

    ELY_ALWAYS_INLINE constexpr ely::Union<Ts..., VariantUninit>&&
    get_union() && noexcept
    {
        return std::move(*this).first();
    }

    ELY_ALWAYS_INLINE constexpr const ely::Union<Ts..., VariantUninit>&&
    get_union() const&& noexcept
    {
        return std::move(*this).first();
    }

    ELY_ALWAYS_INLINE constexpr std::size_t index() const noexcept
    {
        return static_cast<std::size_t>(this->second());
    }

protected:
    ELY_ALWAYS_INLINE constexpr void set_index(std::size_t new_idx) noexcept
    {
        base_::second() = static_cast<variant_index_t<sizeof...(Ts)>>(new_idx);
    }
};

template<ely::detail::Availability A, typename... Ts>
class VariantDestructor;

#define VARIANT_IMPL(availability, destructor, destroy_unchecked)              \
    template<typename... Ts>                                                   \
    class VariantDestructor<availability, Ts...>                               \
        : public VariantStorage<Ts...>                                         \
    {                                                                          \
        friend struct ely::detail::VariantAccess;                              \
                                                                               \
        using base_ = VariantStorage<Ts...>;                                   \
                                                                               \
    public:                                                                    \
        using base_::base_;                                                    \
        VariantDestructor(const VariantDestructor&) = default;                 \
        VariantDestructor(VariantDestructor&&)      = default;                 \
                                                                               \
        destructor                                                             \
                                                                               \
                           VariantDestructor&                                  \
                           operator=(const VariantDestructor&) = default;      \
        VariantDestructor& operator=(VariantDestructor&&) = default;           \
                                                                               \
        using base_::index;                                                    \
                                                                               \
    protected:                                                                 \
        destroy_unchecked                                                      \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             ~VariantDestructor() = default;
             ,
             ELY_ALWAYS_INLINE constexpr void destroy_unchecked() noexcept {});

VARIANT_IMPL(
    ely::detail::Availability::Available,
    ~VariantDestructor() {
        dispatch_index<sizeof...(Ts)>(
            [&](auto i) noexcept {
                constexpr auto I = decltype(i)::value;
                ely::destroy<I>(this->get_union());
            },
            this->index());
    },
    constexpr void destroy_unchecked() noexcept {
        ely::detail::dispatch_index<sizeof...(Ts)>(
            [&](auto i) {
                constexpr auto I = decltype(i)::value;
                ely::destroy<I>(this->get_union());
            },
            this->index());
    });

VARIANT_IMPL(ely::detail::Availability::Unavailable,
             ~VariantDestructor() = delete;
             , void destroy_unchecked() = delete;);

#undef VARIANT_IMPL

template<ely::detail::Availability A, typename... Ts>
class VariantCopyConstruct;

#define VARIANT_IMPL(availability, copy_construct)                              \
    template<typename... Ts>                                                    \
    class VariantCopyConstruct<availability, Ts...>                             \
        : public VariantDestructor<                                             \
              ely::detail::CommonAvailability<Ts...>::destructible,             \
              Ts...>                                                            \
    {                                                                           \
        using base_ = VariantDestructor<                                        \
            ely::detail::CommonAvailability<Ts...>::destructible,               \
            Ts...>;                                                             \
                                                                                \
    public:                                                                     \
        using base_::base_;                                                     \
                                                                                \
        copy_construct                                                          \
                                                                                \
        VariantCopyConstruct(VariantCopyConstruct&&) = default;                 \
                                                                                \
        ~VariantCopyConstruct() = default;                                      \
        VariantCopyConstruct&                                                   \
                              operator=(const VariantCopyConstruct&) = default; \
        VariantCopyConstruct& operator=(VariantCopyConstruct&&) = default;      \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             VariantCopyConstruct(const VariantCopyConstruct&) = default;);
VARIANT_IMPL(
    ely::detail::Availability::Available,
    ELY_ALWAYS_INLINE constexpr VariantCopyConstruct(
        const VariantCopyConstruct&
            other) noexcept((std::is_nothrow_copy_constructible_v<Ts> && ...))
    : base_(VariantUninit{}) {
        this->set_index(other.index());
        dispatch_index<sizeof...(Ts)>(
            [&](auto i) {
                constexpr auto I = decltype(i)::value;
                ely::emplace<I>(this->get_union(),
                                ely::get_unchecked<I>(other));
            },
            this->index());
    });
VARIANT_IMPL(ely::detail::Availability::Unavailable,
             VariantCopyConstruct(const VariantCopyConstruct&) = delete;);

#undef VARIANT_IMPL

template<ely::detail::Availability A, typename... Ts>
class VariantMoveConstruct;

#define VARIANT_IMPL(availability, move_construct)                              \
    template<typename... Ts>                                                    \
    class VariantMoveConstruct<availability, Ts...>                             \
        : public VariantCopyConstruct<                                          \
              ely::detail::CommonAvailability<Ts...>::copy_constructible,       \
              Ts...>                                                            \
    {                                                                           \
        using base_ = VariantCopyConstruct<                                     \
            ely::detail::CommonAvailability<Ts...>::copy_constructible,         \
            Ts...>;                                                             \
                                                                                \
    public:                                                                     \
        using base_::base_;                                                     \
        VariantMoveConstruct(const VariantMoveConstruct&) = default;            \
                                                                                \
        move_construct                                                          \
                                                                                \
            ~VariantMoveConstruct() = default;                                  \
        VariantMoveConstruct&                                                   \
                              operator=(const VariantMoveConstruct&) = default; \
        VariantMoveConstruct& operator=(VariantMoveConstruct&&) = default;      \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             VariantMoveConstruct(VariantMoveConstruct&&) = default;);
VARIANT_IMPL(
    ely::detail::Availability::Available,
    ELY_ALWAYS_INLINE constexpr VariantMoveConstruct(
        VariantMoveConstruct&&
            other) noexcept((std::is_nothrow_move_constructible_v<Ts> && ...))
    : base_(VariantUninit{}) {
        this->set_index(other.index());
        dispatch_index<sizeof...(Ts)>(
            [&](auto i) {
                constexpr auto I = decltype(i)::value;
                ely::emplace<I>(this->get_union(),
                                ely::get_unchecked<I>(std::move(other)));
            },
            this->index());
    });
VARIANT_IMPL(ely::detail::Availability::Unavailable,
             VariantMoveConstruct(VariantMoveConstruct&&) = delete;);

#undef VARIANT_IMPL

template<ely::detail::Availability A, typename... Ts>
class VariantCopyAssign;

#define VARIANT_IMPL(availability, copy_assign)                                \
    template<typename... Ts>                                                   \
    class VariantCopyAssign<availability, Ts...>                               \
        : public VariantMoveConstruct<                                         \
              ely::detail::CommonAvailability<Ts...>::move_constructible,      \
              Ts...>                                                           \
    {                                                                          \
        using base_ = VariantMoveConstruct<                                    \
            ely::detail::CommonAvailability<Ts...>::move_constructible,        \
            Ts...>;                                                            \
                                                                               \
    public:                                                                    \
        using base_::base_;                                                    \
        VariantCopyAssign(const VariantCopyAssign&) = default;                 \
        VariantCopyAssign(VariantCopyAssign&&)      = default;                 \
                                                                               \
        ~VariantCopyAssign() = default;                                        \
                                                                               \
        copy_assign                                                            \
                                                                               \
            VariantCopyAssign&                                                 \
            operator=(VariantCopyAssign&&) = default;                          \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             VariantCopyAssign& operator=(const VariantCopyAssign&) = default;);
VARIANT_IMPL(
    ely::detail::Availability::Available,
    ELY_ALWAYS_INLINE constexpr VariantCopyAssign&
    operator=(const VariantCopyAssign& other) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> && ...)) {
        this->destroy_unchecked();

        this->set_index(other.index());

        dispatch_index<sizeof...(Ts)>(
            [&](auto i) {
                constexpr auto I = decltype(i)::value;
                ely::emplace<I>(this->get_union(),
                                ely::get_unchecked<I>(other));
            },
            this->index());

        return *this;
    });
VARIANT_IMPL(ely::detail::Availability::Unavailable,
             VariantCopyAssign& operator=(const VariantCopyAssign&) = delete;);

#undef VARIANT_IMPL

template<ely::detail::Availability A, typename... Ts>
class VariantMoveAssign;

#define VARIANT_IMPL(availability, move_assign)                                \
    template<typename... Ts>                                                   \
    class VariantMoveAssign<availability, Ts...>                               \
        : public VariantCopyAssign<                                            \
              ely::detail::CommonAvailability<Ts...>::copy_assignable,         \
              Ts...>                                                           \
    {                                                                          \
        using base_ = VariantCopyAssign<                                       \
            ely::detail::CommonAvailability<Ts...>::copy_assignable,           \
            Ts...>;                                                            \
                                                                               \
    public:                                                                    \
        using base_::base_;                                                    \
        VariantMoveAssign(const VariantMoveAssign&) = default;                 \
        VariantMoveAssign(VariantMoveAssign&&)      = default;                 \
                                                                               \
        ~VariantMoveAssign() = default;                                        \
                                                                               \
        VariantMoveAssign& operator=(const VariantMoveAssign&) = default;      \
                                                                               \
        move_assign                                                            \
    }

VARIANT_IMPL(ely::detail::Availability::TriviallyAvailable,
             VariantMoveAssign& operator=(VariantMoveAssign&&) = default;);
VARIANT_IMPL(
    ely::detail::Availability::Available,
    ELY_ALWAYS_INLINE constexpr VariantMoveAssign&
    operator=(VariantMoveAssign&& other) noexcept(
        (std::is_nothrow_move_constructible_v<Ts> && ...)) {
        this->destroy_unchecked();

        this->set_index(other.index());

        dispatch_index<sizeof...(Ts)>(
            [&](auto i) {
                constexpr auto I = decltype(i)::value;
                ely::emplace<I>(this->get_union(),
                                ely::get_unchecked<I>(std::move(other)));
            },
            this->index());

        return *this;
    });
VARIANT_IMPL(ely::detail::Availability::Unavailable,
             VariantMoveAssign& operator=(VariantMoveAssign&&) = delete;);

#undef VARIANT_IMPL

} // namespace detail
template<typename... Ts>
class Variant : public detail::VariantMoveAssign<
                    ely::detail::CommonAvailability<Ts...>::move_assignable,
                    Ts...>
{
    friend ::ely::detail::VariantAccess;

    static constexpr std::size_t variant_size_ = sizeof...(Ts);

    using base_ = detail::VariantMoveAssign<
        ely::detail::CommonAvailability<Ts...>::move_assignable,
        Ts...>;

public:
    using base_::base_;

    Variant()               = default;
    Variant(const Variant&) = default;
    Variant(Variant&&)      = default;

    ~Variant() = default;

    Variant& operator=(const Variant&) = default;
    Variant& operator=(Variant&&) = default;

    template<std::size_t I, typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr Variant(std::in_place_index_t<I>,
                                                 Args&&... args)
        : base_(detail::VariantUninit{})
    {
        this->set_index(I);
        ely::emplace<I>(this->get_union(), static_cast<Args&&>(args)...);
    }

    template<typename T, typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr Variant(std::in_place_type_t<T>,
                                                 Args&&... args)
        : Variant(std::in_place_index<detail::FindElementIndex<T, Ts...>>,
                  static_cast<Args&&>(args)...)
    {
        static_assert(detail::FindElementIndex<T, Ts...> != sizeof...(Ts),
                      "T is not part of the variant");
    }

    template<typename U,
             typename = std::enable_if_t<
                 !std::is_same_v<Variant<Ts...>, ely::remove_cvref_t<U>>>>
    ELY_ALWAYS_INLINE constexpr Variant(U&& u)
        : Variant(std::in_place_index<detail::ResolveOverloadIndex<U, Ts...>>,
                  static_cast<U&&>(u))
    {}

    using base_::get_union;
    using base_::index;

    template<typename F>
    ELY_ALWAYS_INLINE constexpr decltype(auto) visit(F&& fn) &
    {
        using ely::get_unchecked;

        using R = std::invoke_result_t<F,
                                       decltype(get_unchecked<0>(
                                           std::declval<Variant&>()))>;

        return ely::detail::dispatch_index<sizeof...(Ts)>(
            [&](auto i) -> R {
                constexpr auto I = decltype(i)::value;
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(*this));
            },
            index());
    }

    template<typename F>
    ELY_ALWAYS_INLINE constexpr decltype(auto) visit(F&& fn) const&
    {
        using ely::get_unchecked;

        using R = std::invoke_result_t<F,
                                       decltype(get_unchecked<0>(
                                           std::declval<const Variant&>()))>;

        return ely::detail::dispatch_index<sizeof...(Ts)>(
            [&](auto i) -> R {
                constexpr auto I = decltype(i)::value;
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(*this));
            },
            index());
    }

    template<typename F>
    ELY_ALWAYS_INLINE constexpr decltype(auto) visit(F&& fn) &&
    {
        using ely::get_unchecked;

        using R = std::invoke_result_t<F,
                                       decltype(get_unchecked<0>(
                                           std::declval<Variant&&>()))>;

        return ely::detail::dispatch_index<sizeof...(Ts)>(
            [&](auto i) -> R {
                constexpr auto I = decltype(i)::value;
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(std::move(*this)));
            },
            index());
    }

    template<typename F>
    ELY_ALWAYS_INLINE constexpr decltype(auto) visit(F&& fn) const&&
    {
        using ely::get_unchecked;

        using R = std::invoke_result_t<F,
                                       decltype(get_unchecked<0>(
                                           std::declval<const Variant&&>()))>;

        return ely::detail::dispatch_index<sizeof...(Ts)>(
            [&](auto i) -> R {
                constexpr auto I = decltype(i)::value;
                return std::invoke(static_cast<F&&>(fn),
                                   get_unchecked<I>(std::move(*this)));
            },
            index());
    }
};

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr bool
holds_alternative(const Variant<Ts...>& v) noexcept
{
    return ely::detail::dispatch_index<sizeof...(Ts)>(
        [&](auto j) {
            constexpr auto J = decltype(j)::value;
            return I == J;
        },
        v.index());
}

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr bool holds(const Variant<Ts...>& v) noexcept
{
    using ely::holds_alternative;
    return holds_alternative<I>(v);
}

template<typename T, typename... Ts>
ELY_ALWAYS_INLINE constexpr bool
holds_alternative(const Variant<Ts...>& v) noexcept
{
    return ely::detail::dispatch_index<sizeof...(Ts)>(
        [&](auto i) {
            constexpr auto I = decltype(i)::value;

            return std::is_same_v<ely::nth_element_t<I, Ts...>, T>;
        },
        v.index());
}

template<typename T, typename... Ts>
ELY_ALWAYS_INLINE constexpr bool holds(const Variant<Ts...>& v) noexcept
{
    return ely::holds_alternative<T>(v);
}

template<typename F, typename V>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<
    ely::detail::is_derived_from_variant_v<ely::remove_cvref_t<V>>,
    std::invoke_result_t<F,
                         decltype((ely::get_unchecked<0>(std::declval<V>())))>>
visit(F&& fn, V&& v)
{
    using R = std::invoke_result_t<F,
                                   decltype((ely::get_unchecked<0>(
                                       std::declval<V>())))>;
    return ely::detail::dispatch_index<
        ely::detail::VariantAccess::variant_size<V>>(
        [&](auto i) -> R {
            constexpr auto I = decltype(i)::value;
            return std::invoke(static_cast<F&&>(fn),
                               ely::get_unchecked<I>(static_cast<V&&>(v)));
        },
        ely::detail::VariantAccess::index(v));
}

template<typename R, typename F, typename V>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<
    ely::detail::is_derived_from_variant_v<ely::remove_cvref_t<V>>,
    R>
visit(F&& fn, V&& v)
{
    return ely::detail::dispatch_index<
        ely::detail::VariantAccess::variant_size<V>>(
        [&](auto i) -> R {
            constexpr auto I = decltype(i)::value;
            return std::invoke(static_cast<F&&>(fn),
                               ely::get_unchecked<I>(static_cast<V&&>(v)));
        },
        ely::detail::VariantAccess::index(v));
}
} // namespace ely

namespace std
{
template<typename... Ts>
struct variant_size<ely::Variant<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)>
{};
} // namespace std