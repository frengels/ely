#pragma once

#include <utility>

#include "ely/maybe_uninit.hpp"

namespace ely
{
/// pretty much the same as MaybeUninit but guarantees that an object is
/// constructed and not empty
template<typename T>
class ManuallyDestroy : private MaybeUninit<T>
{
private:
    using base_ = MaybeUninit<T>;

public:
    template<typename... Args>
    explicit constexpr ManuallyDestroy(Args&&... args)
        : base_(std::in_place, static_cast<Args&&>(args)...)
    {}

    constexpr T& get() & noexcept
    {
        return this->get_unchecked();
    }

    constexpr const T& get() const& noexcept
    {
        return this->get_unchecked();
    }

    constexpr T&& get() && noexcept
    {
        return std::move(*this).get_unchecked();
    }

    constexpr const T&& get() const&& noexcept
    {
        return std::move(*this).get_unchecked();
    }

    constexpr void destroy() noexcept
    {
        this->destroy_unchecked();
    }
};
} // namespace ely