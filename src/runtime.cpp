#include "ely/runtime.h"

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <charconv>

#include "ast.hpp"

enum struct value_kind
{
    uint,
    sint,
    dbl,
    string_lit,
    int_lit,
    dec_lit,
};

struct ely_value
{
    value_kind kind;
    union
    {
        char     nothing;
        uint64_t uval;
        int64_t  sval;
        double   d;
        struct
        {
            char*  str;
            size_t len;
        } lit;
    } as;

    ely_value(int64_t val) : kind(value_kind::sint)
    {
        as.sval = val;
    }

    ely_value(uint64_t val) : kind(value_kind::uint)
    {
        as.uval = val;
    }

    ely_value(double val) : kind(value_kind::dbl)
    {
        as.d = val;
    }

    ely_value(value_kind kind, const char* str, size_t len) : kind(kind)
    {
        as.lit.str = new char[len + 1];
        std::copy(str, str + len, as.lit.str);
        as.lit.str[len] = '\0';
        as.lit.len      = len;
    }

    ~ely_value()
    {
        switch (kind)
        {
        case value_kind::string_lit:
        case value_kind::int_lit:
        case value_kind::dec_lit:
            delete[] as.lit.str;
            break;
        default:
            break;
        }
    }

    void print(FILE* f) const
    {
        switch (kind)
        {
        case value_kind::dbl:
            fprintf(f, "%f", as.d);
            break;
        case value_kind::sint:
            fprintf(f, PRIi64, as.sval);
            break;
        case value_kind::uint:
            fprintf(f, PRIu64, as.uval);
        default:
            assert(0 && "unhandled printing case");
        }
    }

    size_t to_chars(char* buf, size_t buf_len) const
    {
        std::to_chars_result res;

        switch (kind)
        {
        case value_kind::dbl:
            res = std::to_chars(
                buf, buf + buf_len, as.d, std::chars_format::fixed);
            break;
        case value_kind::sint:
            res = std::to_chars(buf, buf + buf_len, as.uval);
            break;
        case value_kind::uint:
            res = std::to_chars(buf, buf + buf_len, as.uval);
            break;
        case value_kind::int_lit:
        case value_kind::dec_lit:
        case value_kind::string_lit:
            if (buf_len < as.lit.len)
                return -1;
            std::copy(as.lit.str, as.lit.str + as.lit.len, buf);
            return as.lit.len;
            break;
        default:
            assert(0 && "to_chars case not handled for ely_value");
            break;
        }

        if (res.ec != std::errc())
        {
            return -1;
        }

        return res.ptr - buf;
    }
};

struct ely_runtime
{
    ely_value* eval(ely_expr* e)
    {
        switch (e->base.kind)
        {
        case ELY_NODE_LIT_INT: {
            ely_int_literal* ilit = static_cast<ely_int_literal*>(e);
            return new ely_value(value_kind::int_lit, ilit->str, ilit->len);
        }
        break;
        case ELY_NODE_LIT_DEC: {
            ely_dec_literal* dlit = static_cast<ely_dec_literal*>(e);
            return new ely_value(value_kind::dec_lit, dlit->str, dlit->len);
        }
        break;
        case ELY_NODE_LIT_STRING: {
            ely_string_literal* strlit = static_cast<ely_string_literal*>(e);
            return new ely_value(
                value_kind::string_lit, strlit->str, strlit->len);
        }
        default:
            assert(0 && "Unhandled case");
            break;
        }
    }
};

ely_runtime* ely_runtime_create()
{
    return new ely_runtime();
}

ely_value* ely_runtime_eval(ely_runtime* rt, ely_expr* e)
{
    return rt->eval(e);
}

void ely_runtime_destroy(ely_runtime* rt)
{
    delete rt;
}

void ely_value_print(const ely_value* v, FILE* f)
{
    v->print(f);
}

size_t ely_value_to_chars(const ely_value* v, char* str, size_t len)
{
    return v->to_chars(str, len);
}