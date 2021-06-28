#pragma once

#include <memory>

#include "ely/utility.hpp"

namespace ely
{
namespace memory
{
namespace detail
{
template<typename T, typename RefCountT, typename Allocator>
struct SharedPtrBlock
{
private:
    using BlockAlloc =
        typename std::allocator_traits<Allocator>::template rebind_alloc<
            SharedPtrBlock<T, RefCountT, Allocator>>;

public:
    [[no_unique_address]] T          value;
    [[no_unique_address]] RefCountT  ref{1};
    [[no_unique_address]] BlockAlloc alloc;

    template<typename... Args>
    explicit constexpr SharedPtrBlock(const BlockAlloc& alloc, Args&&... args)
        : value(static_cast<Args&&>(args)...), alloc(alloc)
    {}

    constexpr RefCountT add_ref()
    {
        return ++ref;
    }

    constexpr RefCountT release()
    {
        return --ref;
    }
};
} // namespace detail

template<typename T, typename RefCountT, typename Allocator>
class SharedPtr
{
    using alloc_traits = std::allocator_traits<Allocator>;

    static_assert(std::is_same_v<T, typename alloc_traits::value_type>);
    using block_alloc = typename alloc_traits::template rebind_alloc<
        detail::SharedPtrBlock<T, RefCountT, Allocator>>;
    using block_alloc_traits = std::allocator_traits<block_alloc>;
    using block_ptr          = typename block_alloc_traits::pointer;

public:
    using element_type = T;
    using pointer =
        typename std::pointer_traits<block_ptr>::template rebind<element_type>;

private:
    block_ptr p_{};

public:
    SharedPtr() = default;

    template<typename... Args>
    constexpr SharedPtr(const Allocator& alloc, std::in_place_t, Args&&... args)
        : p_([&]() {
              block_alloc balloc{alloc};

              block_ptr ptr = block_alloc_traits::allocate(balloc, 1);

              try
              {
                  block_alloc_traits::construct(balloc,
                                                ely::to_address(ptr),
                                                balloc,
                                                static_cast<Args&&>(args)...);
              }
              catch (...)
              {
                  block_alloc_traits::deallocate(balloc, ptr, 1);
              }

              return ptr;
          }())
    {}

    template<typename... Args>
    explicit constexpr SharedPtr(std::in_place_t, Args&&... args)
        : SharedPtr(Allocator{}, std::in_place, static_cast<Args&&>(args)...)
    {}

    constexpr SharedPtr(const SharedPtr& other) : p_(other.p_)
    {
        ++p_->ref;
    }

    constexpr SharedPtr(SharedPtr&& other) noexcept : p_(other.p_)
    {
        other.p_ = block_ptr{};
    }

    constexpr SharedPtr& operator=(const SharedPtr& other)
    {
        reset();

        p_ = other.p_;
        p_->add_ref();

        return *this;
    }

    constexpr SharedPtr& operator=(SharedPtr&& other) noexcept
    {
        reset();

        p_ = std::exchange(other.p_, block_ptr{});

        return *this;
    }

    ~SharedPtr()
    {
        reset();
    }

    constexpr RefCountT ref_count() const
    {
        return p_->ref;
    }

    constexpr void reset() noexcept
    {
        if (p_)
        {
            if (p_->release() == 0)
            {
                block_alloc alloc{std::move(p_->alloc)};

                block_alloc_traits::destroy(alloc, ely::to_address(p_));
                block_alloc_traits::deallocate(alloc, p_, 1);
            }

            p_ = block_ptr{};
        }
    }

    explicit constexpr operator bool() const noexcept
    {
        return static_cast<bool>(p_);
    }

    constexpr pointer get() const noexcept
    {
        return std::pointer_traits<pointer>::pointer_to(p_->value);
    }

    constexpr pointer operator->() const noexcept
    {
        return get();
    }

    constexpr T& operator*() const noexcept
    {
        return p_->value;
    }

    constexpr void swap(SharedPtr& other) noexcept
    {
        using std::swap;

        swap(p_, other.p_);
    }
};
} // namespace memory

template<typename T, typename RefCountT, typename Allocator = std::allocator<T>>
using SharedPtr = ely::memory::SharedPtr<T, RefCountT, Allocator>;
} // namespace ely