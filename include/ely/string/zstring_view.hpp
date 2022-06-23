#pragma once

#include <iterator>
#include <ranges>
#include <string>

namespace ely
{
template<typename CharT, typename Traits = std::char_traits<CharT>>
class basic_zstring_view
{
    static_assert(std::is_same_v<CharT, typename Traits::char_type>,
                  "CharT must be the same as Traits::char_type");

public:
    using traits_type     = Traits;
    using value_type      = CharT;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using reference       = value_type&;
    using const_reference = const value_type&;
    // using iterator        = pointer;
    // using const_iterator  = const_pointer;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using sentinel        = std::default_sentinel_t;

    class iterator;
    class const_iterator;

private:
    const_pointer zstr_;

public:
    constexpr basic_zstring_view(const CharT* s) : zstr_(s)
    {}

    template<typename Alloc>
    constexpr basic_zstring_view(
        const std::basic_string<value_type, traits_type, Alloc>& s)
        : basic_zstring_view(s.c_str())
    {}

    constexpr const_pointer data() const
    {
        return zstr_;
    }

    constexpr const_reference operator[](size_type pos) const
    {
        return data()[pos];
    }

    constexpr const_reference front() const
    {
        return *zstr_;
    }

    constexpr const_iterator begin() const
    {
        return data();
    }

    constexpr const_iterator cbegin() const
    {
        return begin();
    }

    constexpr sentinel end() const
    {
        return {};
    }

    constexpr sentinel cend() const
    {
        return end();
    }
};

template<typename CharT, typename Traits>
class basic_zstring_view<CharT, Traits>::iterator
{
public:
    using value_type        = CharT;
    using pointer           = const value_type*;
    using reference         = const value_type&;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;
    using iterator_concept  = std::contiguous_iterator_tag;

private:
    const CharT* s_;

public:
    iterator() = default;

    constexpr iterator(const CharT* s) : s_(s)
    {}

    friend bool operator==(const iterator& lhs, const iterator& rhs)  = default;
    friend auto operator<=>(const iterator& lhs, const iterator& rhs) = default;

    friend constexpr bool operator==(const iterator& it,
                                     const std::default_sentinel_t&)
    {
        return !Traits::not_eof(*it);
    }

    friend constexpr bool operator==(const std::default_sentinel_t& s,
                                     const iterator&                it)
    {
        return it == s;
    }

    constexpr pointer data() const
    {
        return s_;
    }

    constexpr pointer c_str() const
    {
        return data();
    }

    constexpr iterator& operator++()
    {
        ++s_;
        return *this;
    }

    constexpr iterator operator++(int)
    {
        iterator res{*this};
        ++*this;
        return res;
    }

    constexpr iterator& operator--()
    {
        --s_;
        return *this;
    }

    constexpr iterator operator--(int)
    {
        iterator res{*this};
        --*this;
        return res;
    }

    constexpr iterator& operator+=(difference_type offset)
    {
        s_ += offset;
        return *this;
    }

    friend constexpr iterator operator+(const iterator& it,
                                        difference_type offset)
    {
        iterator res{it};
        res += offset;
        return res;
    }

    friend constexpr iterator operator+(difference_type offset,
                                        const iterator& it)
    {
        return it + offset;
    }

    constexpr iterator& operator-=(difference_type offset)
    {
        s_ -= offset;
        return *this;
    }

    friend constexpr iterator operator-(const iterator& it,
                                        difference_type offset)
    {
        iterator res{it};
        res -= offset;
        return res;
    }

    friend constexpr difference_type operator-(const iterator& lhs,
                                               const iterator& rhs)
    {
        return lhs.s_ - rhs.s_;
    }

    constexpr reference operator*() const
    {
        return *s_;
    }

    constexpr pointer operator->() const
    {
        return s_;
    }

    constexpr reference operator[](difference_type offset)
    {
        return s_[offset];
    }
};

using zstring_view  = basic_zstring_view<char>;
using wzstring_view = basic_zstring_view<wchar_t>;
} // namespace ely