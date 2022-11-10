#pragma once

#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>

namespace ely
{
struct ilink
{
    ilink* prev{};
    ilink* next{};

    ilink() = default;
    constexpr ilink(ilink* prev, ilink* next) : prev(prev), next(next)
    {}

    constexpr ilink(ilink&& other) noexcept : prev{other.prev}, next{other.next}
    {
        other.next = nullptr;
        other.prev = nullptr;

        if (next && prev)
        {
            next->prev = this;
            prev->next = this;
        }
    }

    constexpr ilink& operator=(ilink&& other) noexcept
    {
        clear();

        prev       = other.prev;
        next       = other.next;
        other.prev = nullptr;
        other.next = nullptr;

        if (next && prev)
        {
            prev->next = this;
            next->prev = this;
        }

        return *this;
    }

    ~ilink()
    {
        clear();
    }

    constexpr void clear() noexcept
    {
        if (next && prev)
        {
            prev->next = next;
            next->prev = prev;
            prev       = nullptr;
            next       = nullptr;
        }
    }
};

struct ilink_access
{
    template<typename T>
    static constexpr ilink& to_ilink(T& t)
    {
        static_assert(std::is_base_of<ilink, T>::value,
                      "must derive from ilink");
        return static_cast<ilink&>(t);
    }

    template<typename T>
    static constexpr const ilink& to_ilink(const T& t)
    {
        static_assert(std::is_base_of<ilink, T>::value,
                      "must derive from ilink");
        return static_cast<const ilink&>(t);
    }

    template<typename T>
    static constexpr T& from_ilink(ilink& il)
    {
        return static_cast<T&>(il);
    }

    static constexpr void remove_ilink(ilink& il)
    {
        il.next->prev = il.prev;
        il.prev->next = il.next;
        il.next       = nullptr;
        il.prev       = nullptr;
    }

    static constexpr void insert_after(ilink& before, ilink& after)
    {
        after.prev        = &before;
        after.next        = before.next;
        before.next->prev = &after;
        before.next       = &after;
    }

    template<typename T>
    static constexpr void insert_after(ilink& before, T& after)
    {
        insert_after(before, to_ilink(after));
    }

    static constexpr void insert_before(ilink& before, ilink& after)
    {
        before.next      = &after;
        before.prev      = after.prev;
        after.prev->next = &before;
        after.prev       = &before;
    }

    template<typename T>
    static constexpr void insert_before(T& before, ilink& after)
    {
        insert_before(to_ilink(before), after);
    }
};

template<typename T>
class ilist_iterator
{
    static constexpr bool is_const = std::is_const<T>::value;
    using ilink_t = std::conditional_t<is_const, const ilink, ilink>;

public:
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

private:
    ilink* p_;

public:
    ilist_iterator() = default;
    explicit constexpr ilist_iterator(ilink_t& ref)
        : p_(std::addressof(const_cast<ilink&>(ref)))
    {}

    // using sfinae to select the conversion from non const to const iterator
    // without overriding trivial copy
    template<typename U,
             typename = std::enable_if_t<is_const &&
                                         std::is_same<U, value_type>::value>>
    constexpr ilist_iterator(const ilist_iterator<U> it)
        : p_(std::addressof(it.impl()))
    {}

    friend constexpr bool operator==(const ilist_iterator& lhs,
                                     const ilist_iterator& rhs) noexcept
    {
        return lhs.p_ == rhs.p_;
    }

    friend constexpr bool operator!=(const ilist_iterator& lhs,
                                     const ilist_iterator& rhs) noexcept
    {
        return lhs.p_ != rhs.p_;
    }

    constexpr ilist_iterator& operator++()
    {
        p_ = p_->next;
        return *this;
    }

    constexpr ilist_iterator operator++(int)
    {
        auto res{*this};
        ++*this;
        return res;
    }

    constexpr ilist_iterator& operator--()
    {
        p_ = p_->prev;
        return *this;
    }

    constexpr ilist_iterator operator--(int)
    {
        auto res{*this};
        --*this;
        return res;
    }

    constexpr reference operator*() const
    {
        return ilink_access::from_ilink<value_type>(*p_);
    }

    constexpr pointer operator->() const
    {
        return std::addressof(**this);
    }

    constexpr ilink& impl() const
    {
        return *p_;
    }

    constexpr void erase()
    {
        ilink_access::remove_ilink(*p_);
    }

    static constexpr ilist_iterator<T> from_ref(T& t)
    {
        return ilist_iterator<T>{ilink_access::to_ilink(t)};
    }
};

template<typename T>
constexpr ilist_iterator<T> as_iterator(T& t)
{
    return ilist_iterator<T>::from_ref(t);
}

template<typename T>
constexpr ilist_iterator<const T> as_iterator(const T& t)
{
    return ilist_iterator<const T>::from_ref(t);
}

template<typename T>
constexpr ilist_iterator<const T> as_const_iterator(const T& t)
{
    return as_iterator(t);
}

/// great care must be exercised when using an intrusive list.
/// The list itself does not own any of the objects inside, therefore when
/// destroying an `ilist` it will not erase the elements. Destroying the `ilist`
/// will invalidate any access to the head of the list, therefore care must be
/// taken to empty the `ilist` before the `ilist` gets destroyed.
template<typename T>
class ilist
{
public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using size_type       = std::size_t;

    using iterator               = ilist_iterator<T>;
    using const_iterator         = ilist_iterator<const T>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    ilink head_{};

public:
    constexpr ilist() : head_{&head_, &head_}
    {}

    ilist(const ilist&) = delete;
    constexpr ilist(ilist&& other) : head_(other.head_.next, other.head_.prev)
    {
        head_.next->prev = &head_;
        head_.prev->next = &head_;
    }

    ilist&           operator=(const ilist&) = delete;
    constexpr ilist& operator=(ilist&& other) = delete;

    ~ilist() = default;

    constexpr iterator begin()
    {
        return iterator{*head_.next};
    }

    constexpr iterator end()
    {
        return iterator{head_};
    }

    constexpr const_iterator begin() const
    {
        return const_iterator{*head_.next};
    }

    constexpr const_iterator end() const
    {
        return const_iterator{head_};
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

    constexpr bool empty() const
    {
        return head_.next == &head_ && head_.prev == &head_;
    }

    constexpr void push_back(T& t)
    {
        ilink_access::insert_before(t, head_);
    }

    constexpr void push_front(T& t)
    {
        ilink_access::insert_after(head_, t);
    }

    constexpr std::size_t count_size() const
    {
        std::size_t i = 0;

        for (const auto& _ : *this)
        {
            ++i;
        }

        return i;
    }
};
} // namespace ely