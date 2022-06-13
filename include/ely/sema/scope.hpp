#pragma once

#include <llvm/ADT/StringMap.h>

#include "ely/ast/ast.hpp"

namespace ely
{
class scope
{
    scope*                       parent_;
    llvm::StringMap<expression*> symbols_;

public:
    scope(scope* parent = nullptr) : parent_(parent)
    {}

    bool is_global() const
    {
        return parent_ == nullptr;
    }

    bool insert(std::string_view name, expression* e)
    {
        return symbols_
            .insert(std::pair<llvm::StringRef, expression*>{
                llvm::StringRef{name.data(), name.size()}, e})
            .second;
    }

    expression* lookup(std::string_view name)
    {
        scope* s = this;
        while (s)
        {
            auto it =
                s->symbols_.find(llvm::StringRef{name.data(), name.size()});
            if (it != s->symbols_.end())
            {
                return it->second;
            }

            s = s->parent();
        }

        return nullptr;
    }

    scope* parent()
    {
        return parent_;
    }
};
} // namespace ely