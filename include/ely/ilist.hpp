#pragma once

#include <memory>

namespace ely
{
namespace detail
{
class EmptyTag;
}

template<typename Tag = detail::EmptyTag>
struct IListBaseHook;
template<typename T, typename Hook = IListBaseHook<>>
class IList;

template<typename Tag>
struct IListBaseHook
{
    IListBaseHook<Tag>* prev{nullptr};
    IListBaseHook<Tag>* next{nullptr};
};

template<typename Hook = IListBaseHook<>>
struct IListHookAccess
{
    IListHookAccess() = delete;

    template<typename T>
    static constexpr Hook& base(T& t) noexcept
    {
        return static_cast<Hook&>(t);
    }

    template<typename T>
    static constexpr const Hook& base(const T& t) noexcept
    {
        return static_cast<const Hook&>(t);
    }

    template<typename T>
    static constexpr T& upcast(Hook& h) noexcept
    {
        return static_cast<T&>(h);
    }

    template<typename T>
    static constexpr const T& upcast(const Hook& h) noexcept
    {
        return static_cast<const T&>(h);
    }

    template<typename T>
    static constexpr Hook* prev(T& t) noexcept
    {
        return base(t).prev;
    }

    template<typename T>
    static constexpr const Hook* prev(const T& t) noexcept
    {
        return base(t).prev;
    }

    template<typename T>
    static constexpr Hook* next(T& t) noexcept
    {
        return base(t).next;
    }

    template<typename T>
    static constexpr const Hook* next(const T& t) noexcept
    {
        return base(t).next;
    }

    template<typename T>
    static constexpr void set_prev(T& t, Hook* h) noexcept
    {
        base(t).prev = h;
    }

    template<typename T>
    static constexpr void set_next(T& t, Hook* h) noexcept
    {
        base(t).next = h;
    }
};

template<typename T, typename Hook>
class IListIterator
{
    friend class IList<std::remove_const_t<T>, std::remove_const_t<Hook>>;

    using HookAccess = IListHookAccess<std::remove_const_t<Hook>>;

public:
    using value_type        = std::remove_const_t<T>;
    using reference         = T&;
    using pointer           = T*;
    using difference_type   = std::size_t;
    using iterator_category = std::bidirectional_iterator_tag;

private:
    Hook* it_{};

public:
    IListIterator() = default;

    explicit constexpr IListIterator(Hook& href) noexcept
        : it_(std::addressof(href))
    {}

    friend constexpr bool operator==(const IListIterator& lhs,
                                     const IListIterator& rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    constexpr IListIterator& operator++() noexcept
    {
        it_ = it_->next;
        return *this;
    }

    constexpr IListIterator operator++(int) noexcept
    {
        IListIterator res{*this};
        ++*this;
        return res;
    }

    constexpr IListIterator& operator--() noexcept
    {
        it_ = it_->prev;
        return *this;
    }

    constexpr IListIterator operator--(int) noexcept
    {
        IListIterator res{*this};
        --*this;
        return res;
    }

    constexpr pointer operator->() const noexcept
    {
        return std::addressof(HookAccess::template upcast<value_type>(*it_));
    }

    constexpr reference operator*() const noexcept
    {
        return *operator->();
    }

    static constexpr IListIterator from_ref(reference ref) noexcept
    {
        return IListIterator{HookAccess::base(ref)};
    }
};

template<typename T, typename Hook>
class IList : private Hook
{
    static_assert(!std::is_reference_v<T>, "T cannot be a reference");
    static_assert(!std::is_const_v<T>, "T cannot be const");

public:
    using value_type      = T;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    using iterator       = IListIterator<T, Hook>;
    using const_iterator = IListIterator<const T, const Hook>;

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using HookAccess = IListHookAccess<Hook>;

private:
    std::size_t sz_{};

public:
    constexpr IList() : Hook{this, this}
    {}

    constexpr IList(IList&& other) noexcept
        : Hook{HookAccess::prev(other), HookAccess::next(other)}
    {
        this->prev->next = this;
        this->next->prev = this;
    }

    constexpr IList& operator=(IList&& other) noexcept
    {
        this->prev->next = this->next;
        this->next->prev = this->prev;

        this->next       = other->next;
        this->prev       = other->prev;
        this->next->prev = this;
        this->prev->next = this;

        other->next = std::addressof(other);
        other->prev = std::addressof(other);

        return *this;
    }

    IList(const IList&) = delete;
    IList& operator=(const IList&) = delete;

    static constexpr iterator iterator_from_ref(reference ref) noexcept
    {
        return iterator{ref};
    }

    static constexpr const_iterator
    iterator_from_ref(const_reference cref) noexcept
    {
        return const_iterator{cref};
    }

    constexpr iterator begin() noexcept
    {
        return iterator{*this->next};
    }

    constexpr iterator end() noexcept
    {
        return iterator{*this};
    }

    constexpr const_iterator begin() const noexcept
    {
        return const_iterator{*this->next};
    }

    constexpr const_iterator end() const noexcept
    {
        return const_iterator{*this};
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

    constexpr std::size_t size() const noexcept
    {
        return sz_;
    }

    constexpr void insert(reference pos, reference ref) noexcept
    {
        Hook& pos_hook = HookAccess::base(pos);

        pos_hook.prev->next = std::addressof(HookAccess::base(ref));
        HookAccess::set_prev(ref, pos_hook.prev);
        pos_hook.prev = std::addressof(HookAccess::base(ref));
        HookAccess::set_next(ref, std::addressof(pos_hook));
        ++sz_;
    }

    constexpr void insert(iterator pos, reference ref) noexcept
    {
        return insert(*pos, ref);
    }

    constexpr void insert_back(T& ref) noexcept
    {
        return insert(end(), ref);
    }

    constexpr void insert_front(T& ref) noexcept
    {
        return insert(begin(), ref);
    }

    constexpr iterator erase(reference ref)
    {
        Hook& h    = HookAccess::base(ref);
        Hook* next = h.next;

        h.next->prev = h.prev;
        h.prev->next = h.next;

        --sz_;

        return iterator_from_ref(HookAccess::template upcast<T>(*next));
    }

    constexpr iterator erase(iterator pos) noexcept
    {
        return erase(*pos);
    }

    constexpr void swap(IList& other) noexcept
    {
        Hook* tmp  = this->prev;
        this->prev = other.prev;
        other.prev = tmp;

        tmp        = this->next;
        this->next = other.next;
        other.next = tmp;

        this->next->prev = this;
        this->prev->next = this;

        other.next->prev = std::addressof(other);
        other.prev->next = std::addressof(other);
    }
};
} // namespace ely