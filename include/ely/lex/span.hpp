#pragma once

#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include "ely/defines.h"

namespace ely
{
template<typename I>
class LexemeSpan;

namespace detail
{
template<typename T, typename... Ts>
inline constexpr bool is_same_as_one_of_v = (std::is_same_v<T, Ts> || ...);
}

/// provides the span for the lexeme, in the case of contiguous iterators we can
/// elide the length since it's just end - start. With non contiguous iterators
/// we don't do this since the minus operation could be expensive.
template<typename I,
         bool ElideLen = detail::is_same_as_one_of_v<
                             I,
                             typename std::string_view::iterator,
                             typename std::string::iterator,
                             typename std::vector<char>::iterator> ||
                         std::is_pointer_v<I>>
class LexemeSpanBase;

template<typename I>
class LexemeSpanBase<I, true>
{
public:
    using iterator  = I;
    using size_type = std::make_unsigned_t<
        typename std::iterator_traits<iterator>::difference_type>;

private:
    iterator start_;
    iterator end_;

public:
    LexemeSpanBase() = default;

    ELY_ALWAYS_INLINE constexpr LexemeSpanBase(iterator  start,
                                               iterator  end,
                                               size_type len) noexcept
        : start_(std::move(start)), end_(std::move(end))
    {
        ELY_ASSERT(std::distance(start_, end_) == len,
                   "The distance between the iterators does not equal the "
                   "given length");
    }

    ELY_ALWAYS_INLINE constexpr iterator begin() const noexcept
    {
        return start_;
    }

    ELY_ALWAYS_INLINE constexpr iterator end() const noexcept
    {
        return end_;
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return static_cast<size_type>(std::distance(begin(), end()));
    }
};

template<typename I>
class LexemeSpanBase<I, false>
{
public:
    using iterator  = I;
    using size_type = std::make_unsigned_t<
        typename std::iterator_traits<iterator>::difference_type>;

private:
    iterator  start_;
    iterator  end_;
    size_type size_;

public:
    LexemeSpanBase() = default;

    ELY_ALWAYS_INLINE constexpr LexemeSpanBase(iterator  start,
                                               iterator  end,
                                               size_type sz) noexcept
        : start_(std::move(start)), end_(std::move(end)), size_(sz)
    {}

    ELY_ALWAYS_INLINE constexpr iterator begin() const noexcept
    {
        return start_;
    }

    ELY_ALWAYS_INLINE constexpr iterator end() const noexcept
    {
        return end_;
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return size_;
    }
};

template<typename I>
class LexemeSpan : public LexemeSpanBase<I>
{
private:
    using base_ = LexemeSpanBase<I>;

public:
    using base_::base_;

    using base_::begin;
    using base_::end;
    using base_::size;
};
} // namespace ely