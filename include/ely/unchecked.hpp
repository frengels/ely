#pragma once

namespace ely
{
/// this type can be passed to eligible functions to bypass any checks
struct UncheckedT
{};

inline constexpr UncheckedT Unchecked{};
} // namespace ely