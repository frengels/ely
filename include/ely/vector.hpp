#pragma once

#include <memory>
#include <vector>

namespace ely
{
template<typename T, typename Alloc = std::allocator<T>>
using Vector = std::vector<T, Alloc>;
}