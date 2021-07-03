#pragma once

#include <memory>
#include <type_traits>

#include "ely/defines.h"
#include "ely/utility.hpp"

namespace ely
{
namespace detail
{
enum class Availability
{
    TriviallyAvailable,
    Available,
    Unavailable
};

enum class UnionConfig
{
    TriviallyDestructible,
    Destructible,
    Undestructible,
    EBO,
};

struct UnionConfigToAvailability
{
    constexpr Availability operator()(UnionConfig config) const noexcept
    {
        switch (config)
        {
        case UnionConfig::EBO:
        case UnionConfig::TriviallyDestructible:
            return Availability::TriviallyAvailable;
        case UnionConfig::Destructible:
            return Availability::Available;
        case UnionConfig::Undestructible:
            return Availability::Unavailable;
        }
    }
};

inline constexpr auto union_config_to_availability =
    UnionConfigToAvailability{};

struct UnionAccess;

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

    static constexpr Availability default_constructible =
        sizeof...(Ts) == 0 ? Availability::TriviallyAvailable :
        std::is_default_constructible_v<ely::nth_element_t<0, Ts...>> ?
                             Availability::Available :
                             Availability::Unavailable;
};

template<ely::detail::Availability A, typename... Ts>
class UnionDestructor;

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
        friend ::ely::detail::UnionAccess;                                     \
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
        ELY_ALWAYS_INLINE explicit constexpr UnionDestructor(                  \
            std::in_place_index_t<0>,                                          \
            Args&&... args)                                                    \
            : first_(static_cast<Args&&>(args)...)                             \
        {}                                                                     \
        template<typename U, typename... Args>                                 \
        ELY_ALWAYS_INLINE explicit constexpr UnionDestructor(                  \
            std::in_place_index_t<0>,                                          \
            std::initializer_list<U> il,                                       \
            Args&&... args)                                                    \
            : first_(il, static_cast<Args&&>(args)...)                         \
        {}                                                                     \
                                                                               \
        template<std::size_t Idx, typename... Args>                            \
        ELY_ALWAYS_INLINE explicit constexpr UnionDestructor(                  \
            std::in_place_index_t<Idx>,                                        \
            Args&&... args)                                                    \
            : rest_(std::in_place_index<Idx - 1>,                              \
                    static_cast<Args&&>(args)...)                              \
        {}                                                                     \
                                                                               \
        template<std::size_t I, typename U, typename... Args>                  \
        ELY_ALWAYS_INLINE explicit constexpr UnionDestructor(                  \
            std::in_place_index_t<I>,                                          \
            std::initializer_list<U> il,                                       \
            Args&&... args)                                                    \
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
                                                                               \
    public:                                                                    \
        template<std::size_t I>                                                \
        ELY_ALWAYS_INLINE constexpr auto&&                                     \
        get_unchecked(std::in_place_index_t<I>) & noexcept                     \
        {                                                                      \
            if constexpr (I == 0)                                              \
            {                                                                  \
                return first_;                                                 \
            }                                                                  \
            else                                                               \
            {                                                                  \
                return rest_.get_unchecked(std::in_place_index<I - 1>);        \
            }                                                                  \
        }                                                                      \
                                                                               \
        template<std::size_t I>                                                \
        ELY_ALWAYS_INLINE constexpr auto&&                                     \
            get_unchecked(std::in_place_index_t<I>) const& noexcept            \
        {                                                                      \
            if constexpr (I == 0)                                              \
            {                                                                  \
                return first_;                                                 \
            }                                                                  \
            else                                                               \
            {                                                                  \
                return rest_.get_unchecked(std::in_place_index<I - 1>);        \
            }                                                                  \
        }                                                                      \
                                                                               \
        template<std::size_t I>                                                \
        ELY_ALWAYS_INLINE constexpr auto&&                                     \
        get_unchecked(std::in_place_index_t<I>) && noexcept                    \
        {                                                                      \
            if constexpr (I == 0)                                              \
            {                                                                  \
                return static_cast<T&&>(first_);                               \
            }                                                                  \
            else                                                               \
            {                                                                  \
                return std::move(rest_).get_unchecked(                         \
                    std::in_place_index<I - 1>);                               \
            }                                                                  \
        }                                                                      \
                                                                               \
        template<std::size_t I>                                                \
        ELY_ALWAYS_INLINE constexpr auto&&                                     \
            get_unchecked(std::in_place_index_t<I>) const&& noexcept           \
        {                                                                      \
            if constexpr (I == 0)                                              \
            {                                                                  \
                return static_cast<const T&&>(first_);                         \
            }                                                                  \
            else                                                               \
            {                                                                  \
                return std::move(rest_).get_unchecked(                         \
                    std::in_place_index<I - 1>);                               \
            }                                                                  \
        }                                                                      \
    }

UNION_IMPL(ely::detail::Availability::TriviallyAvailable,
           ~UnionDestructor() = default;);
UNION_IMPL(ely::detail::Availability::Available, ~UnionDestructor(){});
UNION_IMPL(ely::detail::Availability::Unavailable,
           ~UnionDestructor() = delete;);

#undef UNION_IMPL

template<UnionConfig Config, typename Indices, typename... Ts>
class UnionStorage;

template<UnionConfig Config, typename Indices, typename... Ts>
class UnionStorage
    : private UnionDestructor<union_config_to_availability(Config), Ts...>
{
    friend ::ely::detail::UnionAccess;

    using base_ = UnionDestructor<union_config_to_availability(Config), Ts...>;

public:
    using base_::base_;

protected:
    using base_::get_unchecked;
};

template<std::size_t I, typename T, bool CanErase>
struct UnionBox;

/// this specialization gets used when the trivial and empty element is not the
/// first of its kind. Without this the resulting union cannot be empty. Special
/// care must be taken to never access this Box when accessing Union members.
/// get_unchecked<I>() will need a special check to get the first element of
/// this type
template<std::size_t I, typename T>
struct UnionBox<I, T, true>
{
    static_assert(std::is_empty_v<T>, "T must be empty");
    static_assert(std::is_trivial_v<T>, "T must be trivial");

    UnionBox() = default;

    template<std::size_t J, typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr UnionBox(std::in_place_index_t<J>,
                                                  Args&&...)
    {}
};

template<std::size_t I, typename T>
struct UnionBox<I, T, false> : protected T
{
    static_assert(std::is_empty_v<T>, "T must be empty");
    static_assert(std::is_trivial_v<T>, "T must be trivial");

    UnionBox() = default;

    /// ignore if our index wasn't passed
    template<std::size_t J, typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr UnionBox(std::in_place_index_t<J>,
                                                  Args&&...)
        : UnionBox()
    {}

    template<typename... Args>
    ELY_ALWAYS_INLINE explicit constexpr UnionBox(std::in_place_index_t<I>,
                                                  Args&&... args)
        : T(static_cast<Args&&>(args)...)
    {}
};

template<std::size_t... Is, typename... Ts>
class UnionStorage<UnionConfig::EBO, std::index_sequence<Is...>, Ts...>
    : private UnionBox<Is, Ts, ely::type_index_v<Ts, Ts...> != Is>...
{
    friend ::ely::detail::UnionAccess;

public:
    // this should enable trivial construction
    UnionStorage() = default;

    using UnionBox<Is, Ts, ely::type_index_v<Ts, Ts...> != Is>::UnionBox...;

public:
    template<std::size_t I>
    ELY_ALWAYS_INLINE constexpr ely::nth_element_t<I, Ts...>&
    get_unchecked(std::in_place_index_t<I>) & noexcept
    {
        return static_cast<ely::nth_element_t<I, Ts...>&>(*this);
    }

    template<std::size_t I>
    ELY_ALWAYS_INLINE constexpr const ely::nth_element_t<I, Ts...>&
        get_unchecked(std::in_place_index_t<I>) const& noexcept
    {
        return static_cast<const ely::nth_element_t<I, Ts...>&>(*this);
    }
    template<std::size_t I>
    ELY_ALWAYS_INLINE constexpr ely::nth_element_t<I, Ts...>&&
    get_unchecked(std::in_place_index_t<I>) && noexcept
    {
        return static_cast<ely::nth_element_t<I, Ts...>&&>(*this);
    }
    template<std::size_t I>
    ELY_ALWAYS_INLINE constexpr const ely::nth_element_t<I, Ts...>&&
        get_unchecked(std::in_place_index_t<I>) const&& noexcept
    {
        return static_cast<const ely::nth_element_t<I, Ts...>&&>(*this);
    }
};

struct UnionConfigHelper
{
    constexpr UnionConfig
    operator()(std::initializer_list<bool> trivial_empties,
               std::initializer_list<bool> trivial_destroys,
               std::initializer_list<bool> destructibles) const noexcept
    {
        bool success = true;

        for (auto triv_empty : trivial_empties)
        {
            if (!triv_empty)
            {
                success = false;
                break;
            }
        }

        if (success)
        {
            return UnionConfig::EBO;
        }

        success = true;

        for (auto triv_destroy : trivial_destroys)
        {
            if (!triv_destroy)
            {
                success = false;
                break;
            }
        }

        if (success)
        {
            return UnionConfig::TriviallyDestructible;
        }

        success = true;

        for (auto destructible : destructibles)
        {
            if (!destructible)
            {
                success = false;
                break;
            }
        }

        if (success)
        {
            return UnionConfig::Destructible;
        }

        return UnionConfig::Undestructible;
    }
};

template<typename... Ts>
inline constexpr UnionConfig union_config_v = UnionConfigHelper{}(
    {std::conjunction_v<std::is_trivial<Ts>, std::is_empty<Ts>>...},
    {std::is_trivially_destructible_v<Ts>...},
    {std::is_destructible_v<Ts>...});
} // namespace detail
template<typename... Ts>
class Union
    : private detail::UnionStorage<detail::union_config_v<Ts...>,
                                   std::make_index_sequence<sizeof...(Ts)>,
                                   Ts...>
{
    friend ::ely::detail::UnionAccess;

    using base_ = detail::UnionStorage<detail::union_config_v<Ts...>,
                                       std::make_index_sequence<sizeof...(Ts)>,
                                       Ts...>;

public:
    using base_::base_;

private:
    using base_::get_unchecked;
};

namespace detail
{
struct UnionAccess
{
    template<typename U, std::size_t I>
    ELY_ALWAYS_INLINE static constexpr auto
    get_unchecked(U&& u, std::in_place_index_t<I> idx) noexcept
        -> decltype(auto)
    {
        return static_cast<U&&>(u).get_unchecked(idx);
    }
};

template<typename T>
struct is_union : std::false_type
{};

template<typename... Ts>
struct is_union<ely::Union<Ts...>> : std::true_type
{};
} // namespace detail

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr auto get_unchecked(Union<Ts...>& u) noexcept
    -> decltype(auto)
{
    return ely::detail::UnionAccess::get_unchecked(u, std::in_place_index<I>);
}

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr auto get_unchecked(const Union<Ts...>& u) noexcept
    -> decltype(auto)
{
    return ely::detail::UnionAccess::get_unchecked(u, std::in_place_index<I>);
}

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr auto get_unchecked(Union<Ts...>&& u) noexcept
    -> decltype(auto)
{
    return ely::detail::UnionAccess::get_unchecked(std::move(u),
                                                   std::in_place_index<I>);
}

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr auto get_unchecked(const Union<Ts...>&& u) noexcept
    -> decltype(auto)
{
    return ely::detail::UnionAccess::get_unchecked(std::move(u),
                                                   std::in_place_index<I>);
}

template<std::size_t I, typename... Ts>
ELY_ALWAYS_INLINE constexpr void destroy(ely::Union<Ts...>& u) noexcept
{
    using ty = ely::nth_element_t<I, Ts...>;
    static_assert(std::is_destructible_v<ty>,
                  "selected union variant is not destructible");

    if constexpr (!std::is_trivially_destructible<ty>::value)
    {
        ely::get_unchecked<I>(u).ty::~ty();
    }
}

template<std::size_t I, typename... Ts, typename... Args>
ELY_ALWAYS_INLINE constexpr void emplace(ely::Union<Ts...>& u, Args&&... args)
{
    ::new (static_cast<void*>(std::addressof(ely::get_unchecked<I>(u))))
        ely::nth_element_t<I, Ts...>(static_cast<Args&&>(args)...);
}

template<std::size_t I, typename... Ts, typename U, typename... Args>
ELY_ALWAYS_INLINE constexpr void
emplace(ely::Union<Ts...>& u, std::initializer_list<U> il, Args&&... args)
{
    ::new (static_cast<void*>(std::addressof(ely::get_unchecked<I>(u))))
        ely::nth_element_t<I, Ts...>(il, static_cast<Args&&>(args)...);
}
} // namespace ely
