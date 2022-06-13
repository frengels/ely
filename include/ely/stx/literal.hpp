#pragma once

#include <cstddef>

#include <string>
#include <string_view>

#include <llvm/Support/SMLoc.h>

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
    literal_type type_;
    std::string  str_;
    llvm::SMLoc  loc_;

public:
    literal(literal_type ty, std::string_view str)
        : type_(ty), str_(str), loc_(llvm::SMLoc::getFromPointer(str.data()))
    {}

    llvm::SMLoc location() const
    {
        return loc_;
    }
};

inline ely::literal make_int_literal(std::string_view str)
{
    return ely::literal{literal_type::integer, str};
}

inline ely::literal make_decimal_literal(std::string_view str)
{
    return ely::literal{literal_type::decimal, str};
}

inline ely::literal make_string_literal(std::string_view str)
{
    return ely::literal{literal_type::string, str};
}

inline ely::literal make_char_literal(std::string_view str)
{
    return ely::literal{literal_type::character, str};
}
} // namespace ely
