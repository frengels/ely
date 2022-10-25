#include "ely/string.h"

#include <algorithm>

ely_string ely_string_create(const char* s)
{
    return ely_string_create_len(s, __builtin_strlen(s));
}

ely_string ely_string_create_len(const char* s, size_t len)
{
    ely_string res;

    res.len = len;
    res.s   = new char[len + 1];
    std::copy(s, s + len, res.s);
    res.s[len] = '\0';

    return res;
}

void ely_string_destroy(ely_string s)
{
    delete[] s.s;
}