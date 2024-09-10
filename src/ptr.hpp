#pragma once

#include <utility>

namespace ely {
template <typename T> class ref_ptr {
private:
  T* p_{};

public:
  ref_ptr() = default;
  explicit constexpr ref_ptr(T* p) : p_(p) {}

  constexpr ref_ptr(const ref_ptr& rp) : p_(rp.p_) { p_->acquire(); }
  constexpr ref_ptr(ref_ptr&& rp) noexcept
      : p_(std::exchange(rp.p_, nullptr)) {}

  constexpr ref_ptr& operator=(const ref_ptr& rp) {
    p_->release();

    p_ = rp.p_;

    p_->acquire();
    return *this;
  }
  constexpr ref_ptr& operator=(ref_ptr&& rp) noexcept {
    p_->release();

    p_ = std::exchange(rp.p_, nullptr);

    return *this;
  }

  ~ref_ptr() { clear(); }

  constexpr void reset(T* other = nullptr) noexcept {
    clear();
    p_ = other;
  }

  constexpr T* release() noexcept {
    T* res = p_;
    p_ = nullptr;
    return res;
  }

  constexpr void clear() noexcept {
    if (p_) {
      p_->release();
      p_ = nullptr;
    }
  }

  friend constexpr void swap(ref_ptr& lhs, ref_ptr& rhs) noexcept {
    using std::swap;
    swap(lhs.p_, rhs.p_);
  }
};
} // namespace ely