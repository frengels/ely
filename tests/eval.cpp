#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

#include <ely/ast.h>
#include <ely/context.h>
#include <ely/runtime.h>

std::string_view value{"045.1343"};

TEST_CASE("eval")
{
    SUBCASE("literal")
    {
        auto* rt  = ely_runtime_create();
        auto* ctx = ely_context_create();
        auto* e   = ely_dec_literal_create(ctx, value.data(), value.size());
        auto* v   = ely_runtime_eval(rt, reinterpret_cast<ely_expr*>(e));
        ely_value_print(v, stderr);
    }
}