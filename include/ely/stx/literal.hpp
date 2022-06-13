#pragma once

#include <cstddef>

#include "ely/export.h"
#include "ely/position.hpp"

namespace ely
{
enum struct literal_type : unsigned char
{
    integer,
    decimal,
    string,
    character
};

class literal
{
    ely::position pos_;
    literal_type  type_;
    std::string   str_;

public:
    literal(literal_type ty, std::string str, const ely::position& pos = {})
        : pos_(pos), type_(ty), str_(std::move(str))
    {}

    constexpr const ely::position& pos() const
    {
        return pos_;
    }
};

inline ely::literal make_int_literal(std::string str, const ely::position& pos)
{
    return ely::literal{literal_type::integer, std::move(str), pos};
}

inline ely::literal make_decimal_literal(std::string          str,
                                         const ely::position& pos)
{
    return ely::literal{literal_type::decimal, std::move(str), pos};
}

inline ely::literal make_string_literal(std::string          str,
                                        const ely::position& pos)
{
    return ely::literal{literal_type::string, std::move(str), pos};
}

inline ely::literal make_char_literal(std::string str, const ely::position& pos)
{
    return ely::literal{literal_type::character, std::move(str), pos};
}
} // namespace ely
