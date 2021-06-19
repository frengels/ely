#pragma once

#include <memory>
#include <type_traits>

#include "ely/union.hpp"

namespace ely
{
namespace storage
{
namespace detail
{
// this will always take up at least 1 byte, unless T is empty and trivial
template<typename T>
class MaybeUninit
{
private:
    static constexpr bool zero_size_opt =
        std::is_trivial_v<T> && std::is_empty_v<T>;

    using uninit_type = ely::Union<char, T>;

    uninit_type val_;

public:
    MaybeUninit() = default;

    template<typename... Args>
    explicit(sizeof...(Args) == 0) constexpr MaybeUninit(std::in_place_t,
                                                         Args&&... args)
        : val_(std::in_place_index<1>, static_cast<Args&&>(args)...)
    {}

    template<typename... Args>
    constexpr T& emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        std::construct_at(std::addressof(get_unchecked()),
                          static_cast<Args&&>(args)...);
        return get_unchecked();
    }

    constexpr void destroy() noexcept
    {
        std::destroy_at(std::addressof(get_unchecked()));
    }

    constexpr T& get_unchecked() & noexcept
    {
        return static_cast<uninit_type&>(val_).template get_unchecked<1>();
    }

    constexpr const T& get_unchecked() const& noexcept
    {
        return static_cast<const uninit_type&>(val_)
            .template get_unchecked<1>();
    }

    constexpr T&& get_unchecked() && noexcept
    {
        return static_cast<uninit_type&&>(val_).template get_unchecked<1>();
    }

    constexpr const T&& get_unchecked() const&& noexcept
    {
        return static_cast<const uninit_type&&>(val_)
            .template get_unchecked<1>();
    }
};
} // namespace detail

template<typename T, typename Alloc = std::allocator<T>>
class Heap
{
    using alloc_traits = std::allocator_traits<Alloc>;

public:
    using allocator_type = Alloc;

    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = typename alloc_traits::pointer;
    using const_pointer   = typename alloc_traits::const_pointer;
    using size_type       = typename alloc_traits::size_type;

private:
    pointer   data_;
    size_type size_;
    size_type capacity_;

public:
    Heap() = default;
};

template<typename T, std::size_t Size>
class Stack
{
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using size_type       = std::size_t;

    using iterator               = pointer;
    using const_iterator         = const_pointer;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    detail::MaybeUninit<T> data_[Size]{};
    size_type              size_{};

public:
    Stack() = default;

    Stack(const Stack&) requires(std::is_trivially_copy_constructible_v<T>) =
        default;
    constexpr Stack(const Stack& other) noexcept(
        std::is_nothrow_copy_constructible_v<
            T>) requires(!std::is_trivially_copy_constructible_v<T>)
        : size_(other.size_)
    {
        for (std::size_t i = 0; i != other.size(); ++i)
        {
            data_[i].emplace(other[i]);
        }
    }

    Stack(Stack&&) requires(std::is_trivially_move_constructible_v<T>) =
        default;
    constexpr Stack(Stack&& other) noexcept(
        std::is_nothrow_move_constructible_v<
            T>) requires(!std::is_trivially_move_constructible_v<T>)
        : size_(other.size_)
    {
        for (std::size_t i = 0; i != other.size(); ++i)
        {
            data_[i].emplace(std::move(other[i]));
        }
    }

    Stack& operator                             =(const Stack&) requires(
        std::is_trivially_copy_assignable_v<T>) = default;
    constexpr Stack& operator                   =(const Stack& other) requires(
        !std::is_trivially_copy_assignable_v<T>)
    {
        clear();

        for (std::size_t i = 0; i != other.size(); ++i)
        {
            data_[i].emplace(other[i]);
        }

        size_ = other.size();

        return *this;
    }

    Stack& operator=(Stack&&) requires(std::is_trivially_move_assignable_v<T>) =
        default;
    constexpr Stack&
    operator=(Stack&& other) requires(!std::is_trivially_move_assignable_v<T>)
    {
        clear();

        for (std::size_t i = 0; i != other.size(); ++i)
        {
            data_[i].emplace(std::move(other[i]));
        }

        size_ = other.size();

        return *this;
    }

    ~Stack() requires(std::is_trivially_destructible_v<T>) = default;
    ~Stack() requires(!std::is_trivially_destructible_v<T>)
    {
        clear();
    }

    constexpr pointer data() noexcept
    {
        return std::addressof(data_[0].get_unchecked());
    }

    constexpr const_pointer data() const noexcept
    {
        return std::addressof(data_[0].get_unchecked());
    }

    constexpr iterator begin() noexcept
    {
        iterator{data()};
    }

    constexpr iterator end() noexcept
    {
        return begin() + size();
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator{data()};
    }

    constexpr const_iterator end() const noexcept
    {
        return begin() + size();
    }

    constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator{end()};
    }

    constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator{begin()};
    }

    constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator{end()};
    }

    constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator{begin()};
    }

    template<typename... Args>
    constexpr reference emplace_back(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...>)
    {
        reference ref = data_[size_].emplace(static_cast<Args&&>(args)...);
        ++size_; // increment here to prevent size incrementing on exception
        return ref;
    }

    constexpr size_type size() const noexcept
    {
        return size_;
    }

    constexpr size_type capacity() const noexcept
    {
        return Size;
    }

    constexpr reference operator[](size_type idx) noexcept
    {
        return data_[idx].get_unchecked();
    }

    constexpr const_reference operator[](size_type idx) const noexcept
    {
        return data_[idx].get_unchecked();
    }

    constexpr void clear() noexcept
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            std::for_each(begin(), end(), [](reference x) noexcept {
                std::destroy_at(std::addressof(x));
            });
        }

        size_ = size_type{};
    }
};

template<typename T, std::size_t Size, typename Alloc = std::allocator<T>>
class Hybrid
{};
} // namespace storage
} // namespace ely