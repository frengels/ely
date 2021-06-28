#pragma once

#include <iterator>
#include <list>
#include <type_traits>

namespace ely
{
namespace list
{
template<typename T>
class List;

namespace detail
{
struct BaseListNode
{
    BaseListNode* prev;
    BaseListNode* next;
};

template<typename T>
struct ListNode : public BaseListNode
{
    [[no_unique_address]] T value;

    template<typename... Args>
    constexpr ListNode(BaseListNode* prev, BaseListNode* next, Args&&... args)
        : BaseListNode{prev, next}, value(static_cast<Args&&>(args)...)
    {}
};
} // namespace detail

template<typename T>
class ListIterator
{
    friend class List<std::remove_const_t<T>>;

public:
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::bidirectional_iterator_tag;

private:
    detail::BaseListNode* node_{};

public:
    ListIterator() = default;

private:
    explicit constexpr ListIterator(detail::BaseListNode& node)
        : node_(std::addressof(node))
    {}

public:
    friend constexpr bool operator==(const ListIterator& lhs,
                                     const ListIterator& rhs) noexcept
    {
        return lhs.node_ == rhs.node_;
    }

    friend constexpr bool operator!=(const ListIterator& lhs,
                                     const ListIterator& rhs) noexcept
    {
        return !(lhs == rhs);
    }

    constexpr ListIterator& operator++() noexcept
    {
        node_ = node_->next;
        return *this;
    }

    constexpr ListIterator operator++(int) noexcept
    {
        ListIterator res{*this};
        ++*this;
        return res;
    }

    constexpr ListIterator& operator--() noexcept
    {
        node_ = node_->prev;
        return *this;
    }

    constexpr ListIterator operator--(int) noexcept
    {
        ListIterator res{*this};
        --*this;
        return res;
    }

    constexpr pointer operator->() const noexcept
    {
        return std::addressof(**this);
    }

    constexpr reference operator*() const noexcept
    {
        return static_cast<detail::ListNode<T>*>(node_)->value;
    }
};

template<typename T>
class List
{
    static_assert(!std::is_const_v<T>, "T cannot be const qualified");

public:
    using allocator_type = std::allocator<T>;

private:
    using node_alloc_type = typename std::allocator_traits<
        allocator_type>::template rebind_alloc<detail::ListNode<T>>;
    using alloc_traits = std::allocator_traits<node_alloc_type>;

public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;
    using size_type       = std::size_t;

    using iterator               = ListIterator<T>;
    using const_iterator         = ListIterator<const T>;
    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    detail::BaseListNode                  head_;
    size_type                             len_{};
    [[no_unique_address]] node_alloc_type alloc_{};

public:
    constexpr List() noexcept
        : head_{std::addressof(head_), std::addressof(head_)}
    {}

    constexpr List(List&& other) noexcept
        : head_(std::move(other.head_)), len_(std::move(other.len_)),
          alloc_(std::move(other.alloc_))
    {
        head_.next->prev = std::addressof(head_);
        head_.prev->next = std::addressof(head_);

        other.head_.next = std::addressof(other.head_);
        other.head_.prev = std::addressof(other.head_);
        other.len_       = 0;
    }

    constexpr List& operator=(List&& other) noexcept
    {
        clear();

        head_  = std::move(other.head_);
        len_   = std::move(other.len_);
        alloc_ = std::move(other.alloc_);

        head_.next->prev = std::addressof(head_);
        head_.prev->next = std::addressof(head_);

        other.head_.next = std::addressof(other.head_);
        other.head_.prev = std::addressof(other.head_);
        other.len_       = 0;

        return *this;
    }

    ~List()
    {
        clear();
    }

    constexpr size_type size() const noexcept
    {
        return len_;
    }

    constexpr iterator begin() noexcept
    {
        return iterator{*head_.next};
    }

    constexpr iterator end() noexcept
    {
        return iterator{head_};
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator{*head_.next};
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator{head_};
    }

    constexpr const_iterator cbegin() const noexcept
    {
        return begin();
    }

    constexpr const_iterator cend() const noexcept
    {
        return end();
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

    constexpr const_reverse_iterator crbegin() const noexcept
    {
        return rbegin();
    }

    constexpr const_reverse_iterator crend() const noexcept
    {
        return rend();
    }

    template<typename... Args>
    inline iterator emplace(iterator pos, Args&&... args)
    {
        detail::ListNode<T>* node = alloc_traits::allocate(alloc_, 1);

        try
        {
            alloc_traits::construct(alloc_,
                                    node,
                                    pos.node_->prev,
                                    pos.node_,
                                    static_cast<Args&&>(args)...);
        }
        catch (...)
        {
            alloc_traits::deallocate(alloc_, node, 1);
            throw;
        }

        pos.node_->prev->next = node;
        pos.node_->prev       = node;

        ++len_;
        return iterator{*node};
    }

    template<typename... Args>
    constexpr iterator emplace_back(Args&&... args)
    {
        return emplace(end(), static_cast<Args&&>(args)...);
    }

    constexpr iterator erase(iterator pos) noexcept
    {
        iterator next = std::next(pos);

        pos.node_->next->prev = pos.node_->prev;
        pos.node_->prev->next = pos.node_->next;

        --len_;
        return next;
    }

    constexpr iterator erase(iterator first, iterator last) noexcept
    {
        iterator before_first   = std::prev(first);
        first.node_->prev->next = last.node_;

        iterator it = first;

        while (it != last)
        {
            detail::ListNode<T>* current_node =
                static_cast<detail::ListNode<T>*>(it.node_);

            ++it;

            alloc_traits::destroy(alloc_, current_node);
            alloc_traits::deallocate(alloc_, current_node, 1);
            --len_;
        }

        last.node_->prev = before_first.node_;
        return last;
    }

    constexpr void clear() noexcept
    {
        erase(begin(), end());
        len_ = 0;
    }

    constexpr void swap(List& other) noexcept
    {
        using std::swap;
        swap(head_, other.head_);
        swap(len_, other.len_);
        swap(alloc_, other.alloc_);

        head_.prev->next = std::addressof(head_);
        head_.next->prev = std::addressof(head_);

        other.head_.prev->next = std::addressof(other.head_);
        other.head_.next->prev = std::addressof(other.head_);
    }
};
} // namespace list

template<typename T>
using List = ely::list::List<T>;
} // namespace ely