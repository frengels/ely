#pragma once

#include <memory>
#include <type_traits>

#include "ely/utility.hpp"

namespace ely
{
template<std::size_t I, typename... Ts>
using nth_element_t = std::tuple_element_t<I, std::tuple<Ts...>>;

namespace detail
{
enum class Availability
{
    TriviallyAvailable,
    Available,
    Unavailable
};

template<typename T,
         template<typename>
         class IsTriviallyAvailable,
         template<typename>
         class IsAvailable>
struct Available
    : std::integral_constant<Availability,
                             IsTriviallyAvailable<T>::value ?
                                 Availability::TriviallyAvailable :
                             IsAvailable<T>::value ? Availability::Available :
                                                     Availability::Unavailable>
{};

constexpr Availability
common_availability(std::initializer_list<Availability> availables)
{
    Availability current = Availability::TriviallyAvailable;

    for (auto availability : availables)
    {
        switch (availability)
        {
        case Availability::TriviallyAvailable:
            break;
        case Availability::Available:
            current = Availability::Available;
            break;
        case Availability::Unavailable:
            return Availability::Unavailable;
        default:
            __builtin_unreachable();
        }
    }

    return current;
}

template<typename... Ts>
struct CommonAvailability
{
    static constexpr Availability copy_constructible =
        common_availability({Available<Ts,
                                       std::is_trivially_copy_constructible,
                                       std::is_copy_constructible>::value...});
    static constexpr Availability move_constructible =
        common_availability({Available<Ts,
                                       std::is_trivially_move_constructible,
                                       std::is_move_constructible>::value...});
    static constexpr Availability copy_assignable =
        common_availability({Available<Ts,
                                       std::is_trivially_copy_assignable,
                                       std::is_copy_assignable>::value...});
    static constexpr Availability move_assignable =
        common_availability({Available<Ts,
                                       std::is_trivially_move_assignable,
                                       std::is_move_assignable>::value...});
    static constexpr Availability destructible = common_availability(
        {Available<Ts, std::is_trivially_destructible, std::is_destructible>::
             value...});
};
} // namespace detail

namespace union2
{
template<ely::detail::Availability A, typename... Ts>
class UnionDestructor;

template<typename... Ts>
class Union;

template<ely::detail::Availability A, typename... Ts>
class UnionDestructor;

template<ely::detail::Availability A>
class UnionDestructor<A>
{
public:
    /// only participates if the first element is trivially default
    /// constructible
    UnionDestructor() = default;
};

#define UNION_IMPL(available, destructor)                                      \
    template<typename T, typename... Ts>                                       \
    class UnionDestructor<available, T, Ts...>                                 \
    {                                                                          \
        friend struct Access;                                                  \
                                                                               \
        union                                                                  \
        {                                                                      \
            T                                 first_;                          \
            UnionDestructor<available, Ts...> rest_;                           \
        };                                                                     \
                                                                               \
    public:                                                                    \
        UnionDestructor() = default;                                           \
                                                                               \
        template<typename... Args>                                             \
        explicit constexpr UnionDestructor(std::in_place_index_t<0>,           \
                                           Args&&... args)                     \
            : first_(static_cast<Args&&>(args)...)                             \
        {}                                                                     \
        template<typename U, typename... Args>                                 \
        explicit constexpr UnionDestructor(std::in_place_index_t<0>,           \
                                           std::initializer_list<U> il,        \
                                           Args&&... args)                     \
            : first_(il, static_cast<Args&&>(args)...)                         \
        {}                                                                     \
                                                                               \
        template<std::size_t Idx, typename... Args>                            \
        explicit constexpr UnionDestructor(std::in_place_index_t<Idx>,         \
                                           Args&&... args)                     \
            : rest_(std::in_place_index<Idx - 1>,                              \
                    static_cast<Args&&>(args)...)                              \
        {}                                                                     \
                                                                               \
        template<std::size_t I, typename U, typename... Args>                  \
        explicit constexpr UnionDestructor(std::in_place_index_t<I>,           \
                                           std::initializer_list<U> il,        \
                                           Args&&... args)                     \
            : rest_(std::in_place_index<I - 1>,                                \
                    il,                                                        \
                    static_cast<Args&&>(args)...)                              \
        {}                                                                     \
                                                                               \
        UnionDestructor(const UnionDestructor&) = default;                     \
        UnionDestructor(UnionDestructor&&)      = default;                     \
                                                                               \
        destructor                                                             \
                                                                               \
                         UnionDestructor&                                      \
                         operator=(const UnionDestructor&) = default;          \
        UnionDestructor& operator=(UnionDestructor&&) = default;               \
    }

UNION_IMPL(ely::detail::Availability::TriviallyAvailable,
           ~UnionDestructor() = default;);
UNION_IMPL(ely::detail::Availability::Available, ~UnionDestructor(){});
UNION_IMPL(ely::detail::Availability::Unavailable,
           ~UnionDestructor() = delete;);

#undef UNION_IMPL

template<typename... Ts>
class Union : public UnionDestructor<
                  ely::detail::CommonAvailability<Ts...>::destructible,
                  Ts...>
{
    using base_ =
        UnionDestructor<ely::detail::CommonAvailability<Ts...>::destructible,
                        Ts...>;

public:
    using base_::base_;
};

struct Access
{
    template<typename U>
    static constexpr auto&& get_unchecked(U&& u,
                                          std::in_place_index_t<0>) noexcept
    {
        return static_cast<U&&>(u).first_;
    }

    template<typename U, std::size_t I>
    static constexpr auto&& get_unchecked(U&& u,
                                          std::in_place_index_t<I>) noexcept
    {
        return get_unchecked((static_cast<U&&>(u).rest_),
                             std::in_place_index<I - 1>);
    }
};

namespace detail
{
template<typename T>
struct is_union : std::false_type
{};

template<typename... Ts>
struct is_union<::ely::union2::Union<Ts...>> : std::true_type
{};
} // namespace detail

template<std::size_t I,
         typename U,
         typename =
             std::enable_if_t<detail::is_union<ely::remove_cvref_t<U>>::value>>
constexpr auto&& get_unchecked(U&& u) noexcept
{
    return Access::get_unchecked(static_cast<U&&>(u), std::in_place_index<I>);
}

template<std::size_t I, typename... Ts>
constexpr void destroy(::ely::union2::Union<Ts...>& u) noexcept
{
    using ty = ely::nth_element_t<I, Ts...>;
    static_assert(std::is_destructible_v<ty>,
                  "selected union variant is not destructible");

    if constexpr (!std::is_trivially_destructible<ty>::value)
    {
        get_unchecked<I>(u).ty::~ty();
    }
}

template<std::size_t I, typename... Ts, typename... Args>
constexpr void emplace(::ely::union2::Union<Ts...>& u, Args&&... args)
{
    ::new (static_cast<void*>(std::addressof(get_unchecked<I>(u))))
        ely::nth_element_t<I, Ts...>(static_cast<Args&&>(args)...);
}

template<std::size_t I, typename... Ts, typename U, typename... Args>
constexpr void emplace(::ely::union2::Union<Ts...>& u,
                       std::initializer_list<U>     il,
                       Args&&... args)
{
    ::new (static_cast<void*>(std::addressof(get_unchecked<I>(u))))
        ely::nth_element_t<I, Ts...>(il, static_cast<Args&&>(args)...);
}
} // namespace union2

template<typename... Ts>
using Union = union2::Union<Ts...>;
} // namespace ely
