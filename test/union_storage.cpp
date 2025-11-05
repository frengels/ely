#include <ely/util/union_storage.hpp>

#include <cassert>
#include <type_traits>

void union_storage() {
  {
    auto u0 = ely::union_storage<int>(std::in_place_index<0>, 5);
    static_assert(sizeof(u0) == sizeof(int));
    assert(u0.get(std::in_place_index<0>) == 5);
  }

  {
    auto u0 = ely::union_storage<char, int, float, double>(
        std::in_place_index<2>, 5.f);
    static_assert(std::is_trivially_destructible_v<decltype(u0)>);
    static_assert(sizeof(u0) == sizeof(double));
    assert(u0.get(std::in_place_index<2>) == 5.f);
    u0.destroy(std::in_place_index<2>);
    u0.emplace(std::in_place_index<0>, 'c');
    assert(u0.get(std::in_place_index<0>) == 'c');

    // all trivial so safe to just overwrite
    u0.emplace(std::in_place_index<3>, 5.);
    assert(u0.get(std::in_place_index<3>) == 5.);
  }

  {
    auto u0 =
        ely::union_storage<char, std::string>(std::in_place_index<0>, 'a');
    static_assert(!std::is_trivially_destructible_v<decltype(u0)>);
    u0.emplace(std::in_place_index<1>, "Hello world");
    assert(u0.get(std::in_place_index<1>) == "Hello world");
    u0.destroy(std::in_place_index<1>);
  }
}

#ifndef NO_MAIN
int main() {
  union_storage();
  return 0;
}
#endif