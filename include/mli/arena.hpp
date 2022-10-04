#pragma once

namespace mli {

template <typename T> class arena_ptr {
  friend class arena;

private:
  T *p_;

private:
  explicit constexpr arena_ptr(T *p) : p_(p) {}

public:
  arena_ptr() = default;

  arena_ptr(const arena_ptr &) = default;
  arena_ptr(arena_ptr &&) = default;
  arena_ptr &operator=(const arena_ptr &) = default;
  arena_ptr &operator=(arena_ptr &&) = default;
  ~arena_ptr() = default;

  constexpr T *get() const { return p_; }

  friend bool operator==(const arena_ptr &, const arena_ptr &) = default;
  friend auto operator<=>(const arena_ptr &, const arena_ptr &) = default;

  constexpr T *operator->() const { return p_; }
  constexpr T &operator*() const { return *p_; }
};

/// @brief simple arena for allocating objects
/// This is not optimized at all yet, just using new to allocate instead of an
/// actual arena.
class arena {
private:
  // TODO: store all the allocations somewhere
public:
  arena() = default;

  template <typename T> arena_ptr<T> allocate() {
    void *data = ::operator new(sizeof(T));
    return arena_ptr<T>{static_cast<T *>(data)};
  }
  template <typename T> void deallocate(arena_ptr<T> p) noexcept {}
};
} // namespace mli