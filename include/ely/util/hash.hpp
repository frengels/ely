#pragma once

#include <functional>

namespace ely {
template <typename T>
constexpr std::size_t hash_combine_seed(std::size_t seed, const T& t) noexcept {
  std::hash<T> hasher{};
  seed ^= hasher(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

template <typename T, typename... Rest>
constexpr std::size_t hash_combine_seed(std::size_t seed, const T& t,
                                        const Rest&... rest) noexcept {
  return hash_combine_seed(hash_combine_seed(seed, t), rest...);
}

template <typename T, typename... Rest>
constexpr std::size_t hash_combine(const T& t, const Rest&... rest) noexcept {
  return hash_combine_seed(0, t, rest...);
}
} // namespace ely
