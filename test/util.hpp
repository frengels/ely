#pragma once

#include <fmt/core.h>
#include <source_location>

template <typename T, typename U>
constexpr void check_eq(const T& lhs, const U& rhs,
                        std::source_location loc = {}) {
  if (!(lhs == rhs)) {
    fmt::println("error {}:{}:{}:{}:", loc.file_name(), loc.line(),
                 loc.column(), loc.function_name());
    fmt::println("got: {} == {}", lhs, rhs);
    std::terminate();
  }
}
