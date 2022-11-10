#pragma once

#include <string>
#include <string_view>

namespace ely
{
namespace stx
{
class lit_int
{
    std::string str_;

public:
    lit_int(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class list
{
    ely::ilist<node_base> elements_;

public:
    constexpr auto begin()
    {
        return elements_.begin();
    }

    constexpr auto end()
    {
        return elements_.end();
    }
};
} // namespace stx
} // namespace ely