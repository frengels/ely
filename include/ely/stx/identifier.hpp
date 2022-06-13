#pragma once

#include <string>

#include "ely/export.h"
#include "ely/position.hpp"

namespace ely
{
class ELY_EXPORT identifier
{
    ely::position pos_;
    std::string   str_;
    bool          is_literal{false};

public:
    identifier(std::string str, const ely::position& pos = {})
        : pos_(pos), str_(std::move(str))
    {}

    constexpr const ely::position& pos() const
    {
        return pos_;
    }
};
} // namespace ely
