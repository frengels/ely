#pragma once

#include <type_traits>

namespace ely {
namespace arena {
template <typename T> class forever_ptr {
public:
  using element_type = std::remove_extent_t<T>;
  using difference_type = std::ptrdiff_t;

  template <typename U> using rebind = forever_ptr<U>;

private:
  element_type* p_;

public:
  explicit constexpr forever_ptr(element_type* p) : p_(p) {}

  constexpr element_type* get() const noexcept { return p_; }
  constexpr element_type* operator->() const noexcept { return get(); }
  constexpr element_type& operator*() const noexcept { return *get(); }
  constexpr element_type& operator[](std::size_t idx) const noexcept {
    static_assert(std::is_array_v<T>,
                  "ely::arena::forever_ptr<T>::operator[] is only valid for T "
                  "= array.");
    return p_[idx];
  }

  constexpr operator bool() const noexcept { return p_; }
};

class forever {
public:
  template <typename T> using ptr_type = forever_ptr<T>;

public:
  forever() = default;

  template <typename T, typename... Args>
  constexpr std::enable_if_t<!std::is_array_v<T>, forever_ptr<T>>
  make(Args&&... args) const {
    return forever_ptr<T>(new T(static_cast<Args&&>(args)...));
  }

  template <typename T>
  constexpr std::enable_if_t<std::is_unbounded_array_v<T>, forever_ptr<T>>
  make(std::size_t n) const {
    return forever_ptr<T>(new std::remove_extent_t<T>[n]());
  }

  template <typename T>
  constexpr std::enable_if_t<std::is_bounded_array_v<T>, forever_ptr<T>>
  make() const {
    return forever_ptr<T>(new T());
  }

  template <typename T>
  constexpr std::enable_if_t<!std::is_array_v<T>, forever_ptr<T>>
  make_for_overwrite() const {
    return forever_ptr<T>(new T);
  }

  template <typename T, typename... Args>
  std::enable_if_t<!std::is_array_v<T>, forever_ptr<T>>
  make_for_overwrite(Args&&...) const = delete;

  template <typename T>
  constexpr std::enable_if_t<std::is_unbounded_array_v<T>, forever_ptr<T>>
  make_for_overwrite(std::size_t n) const {
    return forever_ptr<T>(new std::remove_extent_t<T>[n]);
  }

  template <typename T>
  constexpr std::enable_if_t<std::is_bounded_array_v<T>, forever_ptr<T>>
  make_for_overwrite() const {
    return forever_ptr<T>(new T);
  }
};
} // namespace arena
} // namespace ely