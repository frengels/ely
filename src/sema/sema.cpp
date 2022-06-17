#include "ely/sema/sema.hpp"

namespace ely
{
void sema::enter_scope()
{
    current_scope_ = new scope(current_scope_);
}

void sema::leave_scope()
{
    auto p = current_scope_->parent();
    delete current_scope_;
    current_scope_ = p;
}
} // namespace ely