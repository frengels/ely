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
    static constexpr auto node_kind = node_kind::stx_lit_int;

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
    static constexpr auto node_kind = node_kind::stx_lit_dec;

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
    static constexpr auto node_kind = node_kind::stx_lit_str;

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
public:
    static constexpr auto node_kind = node_kind::stx_list;

private:
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

class id_user
{
public:
    static constexpr auto node_kind = node_kind::stx_id_user;

private:
    context     ctx_;
    std::string name_;

public:
    id_user(std::string name) : name_(std::move(name))
    {}

    std::string_view name() const
    {
        return name_;
    }
};

class id_prim
{
public:
    static constexpr auto node_kind = node_kind::stx_id_prim;

private:
    context     ctx_;
    std::string name_;

public:
    id_user(std::string name) : name_(std::move(name))
    {}

    std::string_view name() const
    {
        return name_;
    }
};
} // namespace stx

class syntax
{
    template<typename NodeBase,
             typename F,
             typename = std::enable_if_t<
                 std::is_same_v<node_base, std::remove_cvref_t<NodeBase>>>>
    static constexpr auto visit(NodeBase&& nb, F&& fn)
    {
        switch (b.kind())
        {
        case node_kind::stx_id_user:
            return static_cast<const node<stx::id_user>&>(nb).visit(
                static_cast<F&&>(fn));
        case node_kind::stx_id_prim:
            return static_cast<const node<stx::id_prim>&>(nb).visit(
                static_cast<F&&>(fn));
        case node_kind::stx_list:
            return static_cast<const node<stx::list>&>(nb).visit(
                static_cast<F&&>(fn));
        case node_kind::stx_lit_int:
            return static_cast<const node<stx::lit_int>&>(nb).visit(
                static_cast<F&&>(fn));
        case node_kind::stx_lit_dec:
            return static_cast<const node<stx::lit_dec>&>(nb).visit(
                static_cast<F&&>(fn));
        case node_kind::stx_lit_str:
            return static_cast<const node<stx::lit_string>&>(nb).visit(
                static_cast<F&&>(fn));
        default:
            __builtin_unreachable();
        }
    }
};
} // namespace ely