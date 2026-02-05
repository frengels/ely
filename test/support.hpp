#pragma once

#include <fmt/format.h>

#ifndef __has_feature
#define __has_feature(x) 0
#endif

template <typename... T>
constexpr void println_if_cx(fmt::format_string<T...> fmt, T&&... args) {
  if (!std::is_constant_evaluated())
    fmt::println(fmt, static_cast<T&&>(args)...);
}

#define TEST_HAS_ADDRESS_SANITIZER __has_feature(address_sanitizer)
