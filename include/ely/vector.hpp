#pragma once

#include "ely/defines.h"

#include <iterator>
#include <memory>
#include <vector>

namespace ely
{
// template<typename T, typename Alloc = std::allocator<T>>
// using Vector = std::vector<T, Alloc>;

template<typename T, typename Alloc = std::allocator<T>>
class Vector
{
    using alloc_traits = std::allocator_traits<Alloc>;

public:
    using allocator_type = Alloc;

    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using size_type       = typename alloc_traits::size_type;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    [[no_unique_address]] allocator_type alloc_{};
    pointer                              data_{};
    size_type                            size_{};
    size_type                            capacity_{};

public:
    Vector() = default;

    // TODO add copy constructor

    constexpr Vector(Vector&& other) noexcept
        : alloc_(other.alloc_), data_(other.data_), size_(other.size_),
          capacity_(other.capacity_)
    {
        // TODO make these constructors properly allocator aware
        other.data_     = pointer{};
        other.size_     = size_type{};
        other.capacity_ = size_type{};
    }

    constexpr Vector& operator=(Vector&& other) noexcept
    {
        clear();
        dealloc_data();

        data_     = other.data_;
        size_     = other.size_;
        capacity_ = other.capacity_;

        other.data_     = pointer{};
        other.size_     = size_type{};
        other.capacity_ = size_type{};

        return *this;
    }

    ~Vector()
    {
        clear();
        dealloc_data();
    }

private:
    constexpr void dealloc_data() noexcept
    {
        if (data_)
        {
            alloc_traits::deallocate(alloc_, data_, capacity_);
        }
    }

public:
    constexpr allocator_type get_allocator() const
    {
        return alloc_;
    }

    constexpr pointer data()
    {
        return data_;
    }

    constexpr const_pointer data() const
    {
        return data_;
    }

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    /// don't use if unsure
    constexpr void set_size(size_type sz) noexcept
    {
        size_ = sz;
    }

    constexpr bool empty() const
    {
        return size() == 0;
    }

    constexpr size_type capacity() const noexcept
    {
        return capacity_;
    }

    constexpr void unchecked_reserve(size_type new_capacity)
    {
        pointer new_data = alloc_traits::allocate(alloc_, new_capacity);

        iterator it     = begin();
        iterator end_it = end();
        pointer  new_it = new_data;

        for (; it != end_it; ++it, ++new_it)
        {
            alloc_traits::construct(
                alloc_, std::to_address(new_it), std::move_if_noexcept(*it));
        }

        // TODO provide strong exception guarantee

        clear();

        alloc_traits::deallocate(alloc_, data_, size());
        data_ = new_data;
    }

    constexpr void reserve(size_type new_capacity)
    {
        if (capacity() < new_capacity)
        {
            ELY_MUSTTAIL return unchecked_reserve(new_capacity);
        }
    }

    constexpr iterator begin()
    {
        return iterator{data()};
    }

    constexpr iterator end()
    {
        return begin() + size();
    }

    constexpr const_iterator begin() const
    {
        return const_iterator{data()};
    }

    constexpr const_iterator end() const
    {
        return begin() + size();
    }

    constexpr const_iterator cbegin() const
    {
        return begin();
    }

    constexpr const_iterator cend() const
    {
        return end();
    }

    constexpr reverse_iterator rbegin()
    {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend()
    {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator rbegin() const
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const
    {
        return const_reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const
    {
        return rend();
    }

    constexpr reference front()
    {
        return *begin();
    }

    constexpr const_reference front() const
    {
        return *begin();
    }

    constexpr reference back()
    {
        return *--end();
    }

    constexpr const_reference back() const
    {
        return *--end();
    }

    template<typename... Args>
    constexpr reference emplace_back(Args&&... args)
    {
        if (size() == capacity())
        {
            unchecked_reserve(capacity() * 2 + 8);
        }

        alloc_traits::construct(alloc_,
                                std::to_address(data() + size()),
                                static_cast<Args&&>(args)...);
        reference res = *(begin() + size());
        ++size_;
        return res;
    }

    constexpr void clear() noexcept
    {
        for (auto& element : *this)
        {
            alloc_traits::destroy(alloc_, std::addressof(element));
        }
    }
};
} // namespace ely