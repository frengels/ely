#pragma once

#include <string>
#include <string_view>

#include <llvm/Support/SMLoc.h>

#include "ely/export.h"

namespace ely
{
class ELY_EXPORT identifier
{
    std::string str_;
    bool        is_literal{false};
    llvm::SMLoc loc_;

public:
    identifier(std::string_view str)
        : str_(std::move(str)), loc_(llvm::SMLoc::getFromPointer(str.data()))
    {}

    llvm::SMLoc location() const
    {
        return loc_;
    }
};
} // namespace ely
