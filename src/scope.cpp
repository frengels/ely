#include "ely/scope.h"

#include <cstddef>

#include <string>
#include <unordered_map>

#include "ely/string.h"

struct ely_value;

struct ely_scope
{
    ely_scope* parent;
    ely_string name;

    std::unordered_map<std::string, ely_value*> map_;

    ely_scope(ely_scope* parent, const char* name)
        : parent(parent), name(ely_string_create(name))
    {}

    ~ely_scope()
    {
        ely_string_destroy(name);
    }
};

ely_scope* ely_scope_create(ely_scope* parent, const char* name)
{
    return new ely_scope(parent, name);
}

void ely_scope_destroy(ely_scope* s)
{
    delete s;
}