#pragma once

#include <type_traits>

namespace ely
{
template<typename L>
using token_t = decltype(std::declval<L&>().next());

template<typename T>
using location_t = decltype(std::declval<const T&>().location());
} // namespace ely