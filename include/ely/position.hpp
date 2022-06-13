#pragma once

#include <cstdint>

namespace ely
{
struct position
{
    uint32_t offset;
    uint32_t line;
    uint32_t col;
};
} // namespace ely