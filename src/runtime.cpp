#include "ely/runtime.h"

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include <charconv>

#include "ast.hpp"

enum struct value_kind
{
    uint,
    sint,
    dbl,
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
};

struct ely_runtime
{
    ely_value* eval(ely_expr* e)
    {
        switch (e->base.kind)
        {
        case ELY_NODE_LIT_INT:
            // TODO: produce int value instead of double
        case ELY_NODE_LIT_DEC: {
            // TODO: produce literal rather than double
            ely_dec_literal* dlit = static_cast<ely_dec_literal*>(e);
            double           d;
            auto res = std::from_chars(dlit->str, dlit->str + dlit->len, d);
            assert(res.ptr == dlit->str + dlit->len && "Not a decimal value");
            assert(res.ec != std::errc::result_out_of_range);
            return new ely_value(d);
        }
        break;
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