#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

#include <ely/ast.h>
#include <ely/context.h>
#include <ely/runtime.h>

std::string_view value{"45.1343"};

TEST_CASE("eval")
{
    SUBCASE("decimal_literal")
    {
        constexpr auto buf_len = 32;
        char           buf[buf_len];

        auto* rt  = ely_runtime_create();
        auto* ctx = ely_context_create();
        auto* e   = ely_dec_literal_create(ctx, value.data(), value.size());
        auto* v   = ely_runtime_eval(rt, reinterpret_cast<ely_expr*>(e));
        auto  len = ely_value_to_chars(v, buf, buf_len);
        CHECK_NE(len, size_t(-1));
        CHECK_EQ(std::string_view{buf, len}, value);
    }
}