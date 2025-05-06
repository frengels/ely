#pragma once

#include <cstdint>
#include <memory>
#include <type_traits>
#include <utility>

namespace ely {
namespace mem {
namespace detail {
// holds a value and shared pointer count.
// reference count can be increased and decreased with acquire and release. But
// this object must be freed by the releaser when count hits 0.
template <typename T> class shared_node {
  T val_;
  std::size_t count_;

public:
  template <typename... Args>
  constexpr shared_node(Args&&... args)
      : val_(static_cast<Args&&>(args)...), count_() {}

  constexpr T* get() { return std::addressof(val_); }
  constexpr const T* get() const { return std::addressof(val_); }

  constexpr std::size_t acquire() { return ++count_; }
  constexpr std::size_t release() { return --count_; }
};
} // namespace detail

// thread unsafe shared pointer without weak pointer support
template <typename T> class shared_ptr {
  static_assert(!std::is_array_v<T>, "Arrays are not supported");

public:
  using element_type = T;

private:
  detail::shared_node<T>* p_;

public:
  explicit constexpr shared_ptr(detail::shared_node<T>* p) : p_(p) {
    p_->acquire();
  }

  shared_ptr() = default;
  constexpr shared_ptr(std::nullptr_t) : shared_ptr() {}

  constexpr ~shared_ptr() { clear(); }

  constexpr shared_ptr(const shared_ptr& other) : p_(other.p_) {
    p_->acquire();
  }

  constexpr shared_ptr(shared_ptr&& other)
      : p_(std::exchange(other.p_, nullptr)) {}

  constexpr shared_ptr& operator=(const shared_ptr& other) {
    clear();
    p_ = other.p_;
    p_->acquire();
    return *this;
  }

  constexpr shared_ptr& operator=(shared_ptr&& other) {
    clear();
    p_ = std::exchange(other.p_, nullptr);
    return *this;
  }

  constexpr element_type* get() const noexcept { return p_->get(); }
  constexpr element_type* operator->() const noexcept { return p_->get(); }
  constexpr element_type& operator*() const noexcept { return *p_->get(); }

  constexpr operator bool() const noexcept { return p_; }

  constexpr void clear() noexcept {
    if (p_ && p_->release() == 0) {
      delete p_;
    }
  }

  friend constexpr void swap(const shared_ptr& lhs,
                             const shared_ptr& rhs) noexcept {
    using std::swap;
    swap(lhs.p_, rhs.p_);
  }
};

static_assert(sizeof(shared_ptr<int>) == sizeof(int*),
              "shared_ptr shouldn't be larger than pointer");

template <typename T, typename... Args>
constexpr shared_ptr<T> make_shared(Args&&... args) {
  // TODO: have this use an allocator and be able to pass your own deleter
  detail::shared_node<T>* p =
      new detail::shared_node<T>(static_cast<Args&&>(args)...);
  return shared_ptr<T>{p};
}
} // namespace mem
} // namespace ely