#pragma once

#include <iterator>
#include <limits>

#include "ely/assert.h"
#include "ely/defines.h"
#include "ely/pair.hpp"
#include "ely/utility.hpp"

namespace ely
{
namespace detail
{
template<typename T, typename D>
class SpanBase
{
public:
    using element_type    = T;
    using value_type      = ely::remove_cvref_t<element_type>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using iterator = T*;
    using reverse  = std::reverse_iterator<iterator>;

private:
    pointer data_;

protected:
    ELY_ALWAYS_INLINE explicit constexpr SpanBase(pointer data) noexcept
        : data_(data)
    {}

private:
    ELY_ALWAYS_INLINE constexpr void check_crtp() const noexcept
    {
        static_assert(std::is_base_of_v<SpanBase<T, D>, D>,
                      "D must inherit from SpanBase<D> (crtp)");
    }

    ELY_ALWAYS_INLINE constexpr D& derived() noexcept
    {
        check_crtp();
        return static_cast<D&>(*this);
    }

    ELY_ALWAYS_INLINE constexpr const D& derived() const noexcept
    {
        check_crtp();
        return static_cast<const D&>(*this);
    }
};
} // namespace detail

inline constexpr std::size_t dynamic_extent =
    std::numeric_limits<std::size_t>::max();

template<typename T, std::size_t Extent = dynamic_extent>
class Span
{
public:
    using element_type    = T;
    using value_type      = ely::remove_cvref_t<element_type>;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using pointer         = T*;
    using const_pointer   = const T*;
    using reference       = T&;
    using const_reference = const T&;

    using iterator = T*;
    using reverse_iterator  = std::reverse_iterator<iterator>;

private:
    ely::EBOPair<
        pointer,
        std::conditional_t<Extent == dynamic_extent,
                           size_type,
                           std::integral_constant<std::size_t, Extent>>>
        data_size_;

public:
    ELY_ALWAYS_INLINE constexpr Span() noexcept : data_size_({}, {})
    {}

    ELY_ALWAYS_INLINE explicit constexpr Span(pointer data, size_type sz)
        : data_size_(data, sz)
    {}

    ELY_ALWAYS_INLINE constexpr pointer data() const noexcept
    {
        return data_size_.first();
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return data_size_.second();
    }

    ELY_ALWAYS_INLINE constexpr iterator begin() const noexcept
    {
        return data();
    }

    ELY_ALWAYS_INLINE constexpr iterator end() const noexcept
    {
        return data() + size();
    }

    ELY_ALWAYS_INLINE constexpr reverse_iterator rbegin() const noexcept
    {
        return reverse_iterator(end());
    }

    ELY_ALWAYS_INLINE constexpr reverse_iterator rend() const noexcept
    {
        return reverse_iterator(begin());
    }

    ELY_ALWAYS_INLINE constexpr reference front() const noexcept
    {
        return *begin();
    }

    ELY_ALWAYS_INLINE constexpr reference back() const noexcept
    {
        return *rbegin();
    }

    ELY_ALWAYS_INLINE constexpr reference
    operator[](size_type idx) const noexcept
    {
        ELY_ASSERT(idx < size(), "idx attempting to access Span out of bounds");
        return data()[idx];
    }

    ELY_ALWAYS_INLINE constexpr size_type size_bytes() const noexcept
    {
        return size() * sizeof(element_type);
    }

    ELY_ALWAYS_INLINE constexpr bool empty() const noexcept
    {
        return size() == 0;
    }
};
} // namespace ely

static_assert(sizeof(ely::Span<int, 1>) ==
              sizeof(void*)); // should just be a single pointer
static_assert(sizeof(ely::Span<int>) == 2 * sizeof(void*)); // pointer + size