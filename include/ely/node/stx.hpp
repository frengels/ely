#pragma once

#include <string>
#include <string_view>

#include "ely/node.hpp"

namespace ely
{
namespace stx
{
class context
{
    std::string_view filename_;
    uint32_t         line_;
    uint32_t         col_;

public:
    constexpr context(std::string_view filename, uint32_t line, uint32_t col)
        : filename_(filename), line_(line), col_(col)
    {}
    constexpr context(uint32_t line, uint32_t col)
        : context(std::string_view{}, line, col)
    {}

    constexpr std::string_view filename() const
    {
        return filename_;
    }

    constexpr uint32_t line() const
    {
        return line_;
    }

    constexpr uint32_t col() const
    {
        return col_;
    }
};

class lit_int
{
public:
    static constexpr auto node_kind = node_kind::lit_int;

private:
    context     ctx_;
    std::string str_;

public:
    lit_int(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class lit_dec
{
public:
    static constexpr auto node_kind = node_kind::lit_dec;

private:
    context     ctx_;
    std::string str_;

public:
    lit_dec(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class lit_string
{
public:
    static constexpr auto node_kind = node_kind::lit_str;

private:
    context     ctx_;
    std::string str_;

public:
    lit_string(std::string str) : str_(std::move(str))
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
    list() = default;

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