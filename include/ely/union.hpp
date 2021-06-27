#pragma once

#include <memory>
#include <type_traits>

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
class Access
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
        return get_unchecked(static_cast<U&&>(u), std::in_place_index<I - 1>);
    }
};

template<ely::detail::Availability A, typename... Ts>
class UnionImpl;

template<ely::detail::Availability A>
class UnionImpl<A>
{};

#define UNION_IMPL(available, destructor)                                      \
    template<typename T, typename... Ts>                                       \
    class UnionImpl<available, T, Ts...>                                       \
    {                                                                          \
        union                                                                  \
        {                                                                      \
            T                           first_;                                \
            UnionImpl<available, Ts...> rest_;                                 \
        };                                                                     \
                                                                               \
        template<typename... Args>                                             \
        explicit constexpr UnionImpl(std::in_place_index_t<0>, Args&&... args) \
            : first_(static_cast<Args&&>(args)...)                             \
        {}                                                                     \
                                                                               \
        template<std::size_t Idx, typename... Args>                            \
        explicit constexpr UnionImpl(std::in_place_index_t<Idx>,               \
                                     Args&&... args)                           \
            : rest_(std::in_place_index<Idx - 1>,                              \
                    static_cast<Args&&>(args)...)                              \
        {}                                                                     \
                                                                               \
        UnionImpl(const UnionImpl&) = default;                                 \
        UnionImpl(UnionImpl&&)      = default;                                 \
                                                                               \
        destructor                                                             \
                                                                               \
                   UnionImpl&                                                  \
                   operator=(const UnionImpl&) = default;                      \
        UnionImpl& operator=(UnionImpl&&) = default;                           \
    }

UNION_IMPL(ely::detail::Availability::TriviallyAvailable,
           ~UnionImpl() = default;);
UNION_IMPL(ely::detail::Availability::Available, ~UnionImpl(){});
UNION_IMPL(ely::detail::Availability::Unavailable, ~UnionImpl() = delete;);

#undef UNION_IMPL

template<typename... Ts>
using Union =
    UnionImpl<ely::detail::CommonAvailability<Ts...>::destructible, Ts...>;
} // namespace union2

namespace union_
{
template<typename... Ts>
class Union;

template<>
class Union<>
{};

template<typename T, typename... Rest>
class Union<T, Rest...>
{
private:
    using first_type = T;
    using rest_type  = Union<Rest...>;

    static constexpr auto rest_size = sizeof...(Rest);

    static constexpr bool destructible =
        std::is_destructible_v<T> && (std::is_destructible_v<Rest> && ...);
    static constexpr bool trivially_destructible =
        std::is_trivially_destructible_v<T> &&
        (std::is_trivially_destructible_v<Rest> && ...);

    static constexpr bool copy_construct =
        std::is_copy_constructible_v<T> &&
        (std::is_copy_constructible_v<Rest> && ...);
    static constexpr bool trivially_copy_construct =
        std::is_trivially_copy_constructible_v<T> &&
        (std::is_trivially_copy_constructible_v<Rest> && ...);

    static constexpr bool move_construct =
        std::is_move_constructible_v<T> &&
        (std::is_move_constructible_v<Rest> && ...);
    static constexpr bool trivially_move_construct =
        std::is_trivially_move_constructible_v<T> &&
        (std::is_trivially_move_constructible_v<Rest> && ...);

    static constexpr bool copy_assign =
        std::is_copy_assignable_v<T> &&
        (std::is_copy_assignable_v<Rest> && ...);
    static constexpr bool trivially_copy_assign =
        std::is_trivially_copy_assignable_v<T> &&
        (std::is_trivially_copy_assignable_v<Rest> && ...);

    static constexpr bool move_assign =
        std::is_move_assignable_v<T> &&
        (std::is_move_assignable_v<Rest> && ...);
    static constexpr bool trivially_move_assign =
        std::is_trivially_move_assignable_v<T> &&
        (std::is_trivially_move_assignable_v<Rest> && ...);

private:
    union
    {
        first_type first;
        rest_type  rest;
    };

public:
    Union() = default;

    template<std::size_t I, typename... Args>
    explicit constexpr Union(std::in_place_index_t<I>,
                             Args&&... args) requires(I == 0)
        : first(static_cast<Args&&>(args)...)
    {}

    template<std::size_t I, typename... Args>
    explicit constexpr Union(std::in_place_index_t<I>,
                             Args&&... args) requires(I != 0)
        : rest(std::in_place_index<I - 1>, static_cast<Args&&>(args)...)
    {}

    Union(const Union&) requires(copy_construct&& trivially_copy_construct) =
        default;
    Union(const Union&) requires(!copy_construct) = delete;
    constexpr Union(const Union&) noexcept
        requires(copy_construct && !trivially_copy_construct)
    {}

    Union(Union&&) requires(move_construct&& trivially_move_construct) =
        default;
    Union(Union&&) requires(!move_construct) = delete;
    constexpr Union(Union&&) noexcept
        requires(move_construct && !trivially_move_construct)
    {}

    Union& operator                          =(const Union&) requires(
        copy_assign&& trivially_copy_assign) = default;
    Union&           operator=(const Union&) requires(!copy_assign) = delete;
    constexpr Union& operator=(const Union&) noexcept
        requires(copy_assign && !trivially_copy_assign)
    {
        return *this;
    }

    Union&
    operator=(Union&&) requires(move_assign&& trivially_move_assign) = default;
    Union&           operator=(Union&&) requires(!move_assign) = delete;
    constexpr Union& operator=(Union&&) noexcept
        requires(move_assign && !trivially_move_assign)
    {
        return *this;
    }

    ~Union() requires(destructible&& trivially_destructible) = default;
    ~Union() requires(destructible && !trivially_destructible)
    {}
    ~Union() requires(!destructible) = delete;

    template<std::size_t I>
    constexpr T& get_unchecked() & noexcept requires(I == 0)
    {
        return static_cast<T&>(first);
    }

    template<std::size_t I>
    constexpr const T& get_unchecked() const& noexcept requires(I == 0)
    {
        return static_cast<const T&>(first);
    }

    template<std::size_t I>
    constexpr T&& get_unchecked() && noexcept requires(I == 0)
    {
        return static_cast<T&&>(first);
    }

    template<std::size_t I>
    constexpr const T&& get_unchecked() const&& noexcept requires(I == 0)
    {
        return static_cast<const T&&>(first);
    }

    template<std::size_t I>
    constexpr auto get_unchecked() & noexcept
        -> decltype(static_cast<rest_type&>(rest)
                        .template get_unchecked<I - 1>()) requires(I != 0)
    {
        return static_cast<rest_type&>(rest).template get_unchecked<I - 1>();
    }

    template<std::size_t I>
    constexpr auto get_unchecked() const& noexcept
        -> decltype(static_cast<const rest_type&>(rest)
                        .template get_unchecked<I - 1>()) requires(I != 0)
    {
        return static_cast<const rest_type&>(rest)
            .template get_unchecked<I - 1>();
    }

    template<std::size_t I>
    constexpr auto get_unchecked() && noexcept
        -> decltype(static_cast<rest_type&&>(rest)
                        .template get_unchecked<I - 1>()) requires(I != 0)
    {
        return static_cast<rest_type&&>(rest).template get_unchecked<I - 1>();
    }

    template<std::size_t I>
    constexpr auto get_unchecked() const&& noexcept
        -> decltype(static_cast<const rest_type&&>(rest)
                        .template get_unchecked<I - 1>()) requires(I != 0)
    {
        return static_cast<const rest_type&&>(rest)
            .template get_unchecked<I - 1>();
    }

    template<std::size_t I, typename... Args>
    constexpr T& emplace(Args&&... args) requires(I == 0)
    {
        return *std::construct_at<T>(std::addressof(first),
                                     static_cast<Args&&>(args)...);
    }

    template<std::size_t I, typename... Args>
    constexpr decltype(auto) emplace(Args&&... args) requires(I != 0)
    {
        return rest.template emplace<I - 1>(static_cast<Args&&>(args)...);
    }

    template<std::size_t I>
    constexpr void destroy() noexcept requires(I == 0)
    {
        first.~T();
    }

    template<std::size_t I>
    constexpr void destroy() noexcept requires(I != 0)
    {
        rest.template destroy<I - 1>();
    }
};
} // namespace union_

template<typename... Ts>
using Union = union_::Union<Ts...>;
} // namespace ely