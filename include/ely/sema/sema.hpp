#pragma once

#include "ely/sema/scope.hpp"

namespace ely
{
class sema
{
    scope* current_scope_;

public:
    void enter_scope();
    void leave_scope();
};
} // namespace ely