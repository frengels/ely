#include <ely/util/uniquer.hpp>

#include <cassert>

class data {
private:
  struct data_storage {
    int x, y;
    std::string name;

    using key_type = std::tuple<int, int>;

  public:
    data_storage(int x, int y, std::string name)
        : x(x), y(y), name(std::move(name)) {}

    static key_type get_key(int x, int y, const std::string&) {
      return key_type{x, y};
    }
  };

public:
  using storage_type = data_storage;

private:
  const storage_type* impl_;

public:
  data() = default;
  // construct from store pointer
  constexpr data(const storage_type* store) : impl_(store) {}

  // simple pointer equality
  friend constexpr bool operator==(const data& lhs, const data& rhs) {
    return lhs.impl_ == rhs.impl_;
  }

  constexpr int x() const { return impl_->x; }
  constexpr int y() const { return impl_->y; }
  constexpr std::string_view name() const { return impl_->name; }
};

// a simple use case for storageuniquer
template <typename T> bool test_uniquer() {

  auto uniq = ely::uniquer<T>{};
  T d0 = uniq.get_or_emplace(4, 5, "Hello");
  T d1 = uniq.get_or_emplace(4, 5, "Bye");

  assert(d0 == d1);
  assert(d0.name() == "Hello");
  assert(d1.name() == "Hello");

  T d2 = uniq.get_or_emplace(5, 4, "Yes");

  assert(d2 != d0);

  return true;
}

void uniquer() { assert(test_uniquer<data>()); }

#ifndef NO_MAIN
int main() {
  uniquer();

  return 0;
}
#endif
