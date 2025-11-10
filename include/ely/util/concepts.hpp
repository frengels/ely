#pragma once

#include <concepts>

// This file defines a number of valuable utility concepts

namespace ely {
template <typename T, typename... Us>
concept any_of = (std::same_as<T, Us> || ...);
}