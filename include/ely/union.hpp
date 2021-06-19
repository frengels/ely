#pragma once

#include <type_traits>

namespace ely
{
template<typename... Args>
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
    explicit(sizeof...(Args) == 0) constexpr Union(std::in_place_index_t<I>,
                                                   Args&&... args) requires(I ==
                                                                            0)
        : first(static_cast<Args&&>(args)...)
    {}

    template<std::size_t I, typename... Args>
    explicit(sizeof...(Args) == 0) constexpr Union(std::in_place_index_t<I>,
                                                   Args&&... args) requires(I !=
                                                                            0)
        : rest(std::in_place_index<I - 1>, static_cast<Args&&>(args)...)
    {}

    Union(const Union&) requires(copy_construct&& trivially_copy_construct) =
        default;
    Union(const Union&) requires(!copy_construct ||
                                 !trivially_copy_construct) = delete;

    Union(Union&&) requires(move_construct&& trivially_move_construct) =
        default;
    Union(Union&&) requires(!move_construct ||
                            !trivially_move_construct) = delete;

    Union& operator                          =(const Union&) requires(
        copy_assign&& trivially_copy_assign) = default;
    Union& operator=(const Union&) requires(!copy_assign ||
                                            !trivially_copy_assign) = delete;

    Union&
    operator=(Union&&) requires(move_assign&& trivially_move_assign) = default;
    Union& operator=(Union&&) requires(!move_assign ||
                                       !trivially_move_assign) = delete;

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
};
} // namespace ely