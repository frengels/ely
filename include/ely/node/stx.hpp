#pragma once

#include <string>
#include <string_view>

#include "ely/ilist.hpp"

namespace ely
{
class node;
namespace stx
{
class context
{
    std::string_view filename_;
    uint32_t         line_;
    uint32_t         col_;

public:
    context() = default;

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
private:
    context     ctx_;
    std::string str_;

public:
    lit_int() = default;

    lit_int(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class lit_dec
{
private:
    context     ctx_;
    std::string str_;

public:
    lit_dec() = default;

    lit_dec(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class lit_string
{
private:
    context     ctx_;
    std::string str_;

public:
    lit_string() = default;

    lit_string(std::string str) : str_(std::move(str))
    {}

    std::string_view str() const
    {
        return str_;
    }
};

class list
{
private:
    ely::ilist<node> elements_;

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

class id_user
{
private:
    context     ctx_;
    std::string name_;

public:
    id_user() = default;

    id_user(std::string name) : name_(std::move(name))
    {}

    std::string_view name() const
    {
        return name_;
    }
};

class id_prim
{
private:
    context     ctx_;
    std::string name_;

public:
    id_prim() = default;

    id_prim(std::string name) : name_(std::move(name))
    {}

    std::string_view name() const
    {
        return name_;
    }
};
} // namespace stx
} // namespace ely