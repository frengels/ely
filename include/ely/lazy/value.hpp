#pragma once

namespace ely {
template <typename T, typename Res>
concept lazy =
  requires requires(T&& t) {
    { static_cast<T&&>(t).compute() } -> std::convertible_to<Res>;
  };
} // namespace ely