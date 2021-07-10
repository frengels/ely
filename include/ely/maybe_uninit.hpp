#pragma once

#include <type_traits>

#include "ely/union.hpp"

namespace ely
{
namespace detail
{
struct Uninit_
{};
} // namespace detail

template<typename T>
class MaybeUninit : private ely::Union<detail::Uninit_, T>
{
private:
    using base_ = ely::Union<detail::Uninit_, T>;

public:
    MaybeUninit() = default;

    template<typename... Args>
    explicit constexpr MaybeUninit(std::in_place_t, Args&&... args)
        : base_(std::in_place_index<1>, static_cast<Args&&>(args)...)
    {}

    constexpr T& get_unchecked() & noexcept
    {
        return ely::get_unchecked<1>(static_cast<base_&>(*this));
    }

    constexpr const T& get_unchecked() const& noexcept
    {
        return ely::get_unchecked<1>(static_cast<base_&>(*this));
    }

    constexpr T&& get_unchecked() && noexcept
    {
        return ely::get_unchecked<1>(static_cast<base_&>(*this));
    }

    constexpr const T&& get_unchecked() const&& noexcept
    {
        return ely::get_unchecked<1>(static_cast<base_&>(*this));
    }

    template<typename... Args>
    constexpr void emplace_unchecked(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        ely::emplace<1>(*this, static_cast<Args&&>(args)...);
    }

    constexpr void destroy_unchecked() noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            get_unchecked().~T();
        }
    }
};
} // namespace ely