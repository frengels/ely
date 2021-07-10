#pragma once

#include "ely/defines.h"
#include "ely/maybe_uninit.hpp"
#include "ely/utility.hpp"

#include <iterator>
#include <memory>
#include <vector>

namespace ely
{
// template<typename T, typename Alloc = std::allocator<T>>
// using Vector = std::vector<T, Alloc>;

namespace vector
{
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
        destroy_all();
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
                alloc_, ely::to_address(new_it), std::move_if_noexcept(*it));
        }

        // TODO provide strong exception guarantee

        destroy_all();
        dealloc_data();

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
                                ely::to_address(data() + size()),
                                static_cast<Args&&>(args)...);
        reference res = *(begin() + size());
        ++size_;
        return res;
    }

private:
    constexpr void destroy_all() noexcept
    {
        std::for_each(begin(), end(), [&](reference x) noexcept {
            alloc_traits::destroy(alloc_, std::addressof(x));
        });
    }

public:
    constexpr void clear() noexcept
    {
        destroy_all();
        size_ = size_type{};
    }
};

template<std::size_t N>
using smallest_size_t = std::conditional_t<
    N <= std::numeric_limits<uint8_t>::max(),
    uint8_t,
    std::conditional_t<
        N <= std::numeric_limits<uint16_t>::max(),
        uint16_t,
        std::conditional_t<N <= std::numeric_limits<uint32_t>::max(),
                           uint32_t,
                           uint64_t>>>;

template<typename T, std::size_t N>
class StaticVectorStorage
{
protected:
    ely::MaybeUninit<T[N]> data_;
    smallest_size_t<N>     size_;

public:
    StaticVectorStorage() = default;

    constexpr std::size_t size() const noexcept
    {
        return static_cast<std::size_t>(size_);
    }

    constexpr T* data() noexcept
    {
        return data_.get_unchecked();
    }

    constexpr const T* data() const noexcept
    {
        return data_.get_unchecked();
    }
};

template<typename T,
         std::size_t N,
         bool        TrivialDestroy = std::is_trivially_destructible_v<T>>
class StaticVectorDestructible;

#define STATIC_VEC_IMPL(trivial_destruct, destructor, clear_fn)                \
    template<typename T, std::size_t N>                                        \
    class StaticVectorDestructible<T, N, trivial_destruct>                     \
        : public StaticVectorStorage<T, N>                                     \
    {                                                                          \
        using base_ = StaticVectorStorage<T, N>;                               \
                                                                               \
    public:                                                                    \
        using base_::base_;                                                    \
                                                                               \
        StaticVectorDestructible(const StaticVectorDestructible&) = default;   \
        StaticVectorDestructible(StaticVectorDestructible&&)      = default;   \
                                                                               \
        StaticVectorDestructible&                                              \
        operator=(const StaticVectorDestructible&) = default;                  \
        StaticVectorDestructible&                                              \
        operator=(StaticVectorDestructible&&) = default;                       \
                                                                               \
        destructor                                                             \
                                                                               \
            clear_fn                                                           \
    }

STATIC_VEC_IMPL(true, ~StaticVectorDestructible() = default;
                , constexpr void clear() noexcept {});
STATIC_VEC_IMPL(
    false,
    ~StaticVectorDestructible() { clear(); },
    constexpr void clear() noexcept { this->data_.destroy_unchecked(); });

#undef STATIC_VEC_IMPL

template<typename T, std::size_t N>
class StaticVector : public StaticVectorDestructible<T, N>
{
    using base_ = StaticVectorDestructible<T, N>;

public:
    using value_type      = T;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = value_type*;
    using const_pointer   = const value_type*;
    using size_type       = std::size_t;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public:
    StaticVector() = default;

    using base_::data;
    using base_::size;

    constexpr size_type capacity() const noexcept
    {
        return N;
    }

    constexpr reference operator[](size_type idx) noexcept
    {
        return data()[idx];
    }

    constexpr const_reference operator[](size_type idx) const noexcept
    {
        return data()[idx];
    }

    constexpr iterator begin() noexcept
    {
        return data();
    }

    constexpr iterator end() noexcept
    {
        return data() + size();
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return data();
    }

    constexpr const_iterator cend() const noexcept
    {
        return data();
    }

    constexpr const_iterator begin() const noexcept
    {
        return cbegin();
    }

    constexpr const_iterator end() const noexcept
    {
        return cend();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return const_reverse_iterator{cend()};
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return const_reverse_iterator{cbegin()};
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return crbegin();
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return crend();
    }

    template<typename... Args>
    T& emplace_back(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        auto old_sz = size();
        new (static_cast<void*>(data() + size()))
            T(static_cast<Args&&>(args)...);

        ++this->size_;
        return data()[old_sz];
    }

    void pop_back() noexcept
    {
        --this->size_;
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            data()[size()].~T();
        }
    }
};
} // namespace vector

template<typename T>
using Vector = vector::Vector<T, std::allocator<T>>;

template<typename T, std::size_t N>
using StaticVector = vector::StaticVector<T, N>;

} // namespace ely