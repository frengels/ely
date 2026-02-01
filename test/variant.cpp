#include <ely/util/dispatch_index.hpp>
#include <ely/util/isa.hpp>
#include <ely/util/variant.hpp>
#include <ely/util/visit.hpp>

#include <cassert>
#include <string>
#include <type_traits>

struct empty_t {};

constexpr bool test_dispatch() {
  auto i = ely::dispatch_index<10>(
      5, []<std::size_t I>(std::in_place_index_t<I>) { return I; });
  assert(i == 5);
  return true;
}

//
// Compile-time tests
//
constexpr bool constexpr_visit_tests() {
  // default-constructed variant<int,double> holds int(0)
  {
    constexpr ely::variant<int, double> v{};
    struct Visitor {
      constexpr double operator()(int x) const { return x + 0.5; }
      constexpr double operator()(double x) const { return x + 0.5; }
    };
    static_assert(ely::visit(Visitor{}, v) == 0.5);
  }

  // construct variant with index 1 (double) and verify visit behaviour
  {
    constexpr ely::variant<int, double> v(std::in_place_index<1>, 2.5);
    struct Visitor {
      constexpr double operator()(int x) const { return x + 1.0; }
      constexpr double operator()(double x) const { return x * 2.0; }
    };
    static_assert(ely::visit(Visitor{}, v) == 5.0);
  }

  return true;
}

static_assert(constexpr_visit_tests(), "constexpr visit tests failed");

//
// Runtime tests
//
inline void runtime_tests() {
  // variant holding std::string; visitor returns length
  {
    ely::variant<int, std::string> v(std::in_place_index<1>, std::string("hi"));
    bool called = false;
    auto res = ely::visit(
        [&](auto& val) -> std::size_t {
          using T = std::remove_reference_t<decltype(val)>;
          if constexpr (std::is_same_v<T, int>) {
            called = true;
            return static_cast<std::size_t>(val);
          } else {
            called = true;
            return val.size();
          }
        },
        v);
    assert(called && "visitor was not invoked");
    assert(res == 2 && "unexpected string length from visit");
  }

  // variant holding an integer; forwarding lambda increments and returns
  {
    ely::variant<int, long> v(std::in_place_index<0>, 42);
    auto res = ely::visit([](auto&& x) -> long { return x + 1; }, v);
    assert(res == 43 && "unexpected arithmetic result from visit");
  }

  {
    // construct
    struct x {};
    struct y {};
    struct z {};

    auto v = ely::variant<x, y, z>{z{}};
  }
}

void overload() {
  struct x {};
  struct y {};
  struct z {
    z(int);
  };
  static_assert(std::is_same_v<x, ely::resolve_overload_t<const x&, x, y, z>>,
                "const & should resolve to x");
  static_assert(std::is_same_v<y, ely::resolve_overload_t<y&&, x, y, z>>,
                "&& should resolve to y");
  static_assert(std::is_same_v<z, ely::resolve_overload_t<int, x, y, z>>);
}

void isa() {
  ely::variant<int, int*, const int, const int*, const int*> v;
  v.emplace(std::in_place_index<0>, 5);
  assert(ely::isa<int>(v));

  const int i = 10;
  v.emplace(std::in_place_index<2>, i);
  assert(ely::isa<const int>(v));

  int j =5;
  v.emplace(std::in_place_index<1>, &j);
  assert(ely::isa<int*>(v));

  v.emplace(std::in_place_index<4>, &i);
  assert(ely::isa<const int*>(v));
}

// Test entrypoint used by test runner
void variant() {
  assert(test_dispatch());
  static_assert(test_dispatch());

  // this should really be equal in size as it doesn't require an index
  static_assert(sizeof(ely::variant<int>) == sizeof(int));

  // empty type sanity check
  static_assert(std::is_empty_v<empty_t>);

  // runtime tests
  runtime_tests();
}

#ifndef NO_MAIN
int main() {
  overload();
  isa();
  variant();
  return 0;
}
#endif
