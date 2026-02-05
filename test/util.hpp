#pragma once

#include <fmt/core.h>

#define CHECK_EQ(a, b)                                                         \
  do {                                                                         \
    if (!(a == b)) {                                                           \
      fmt::println("failed: " #a " == " #b);                                   \
      fmt::println("got: {} == {}", a, b);                                     \
    }                                                                          \
  } while (0)

