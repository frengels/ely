#include <ely/util/uniquer.hpp>

#include <cassert>

class data {
private:
  struct data_storage {
    int x, y;
    std::string name;

    using key_type = ely::tuple<int, int>;

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

  constexpr const storage_type* get_storage() const { return impl_; }

  constexpr int x() const { return impl_->x; }
  constexpr int y() const { return impl_->y; }
  constexpr std::string_view name() const { return impl_->name; }
};

struct data0 {
  struct storage_type {
    int i;
    using key_type = int;

    static constexpr int get_key(int i) { return i; }
  };

  const storage_type* impl;

  constexpr const storage_type* get_storage() const { return impl; }
};

struct data1 {
  struct storage_type {
    float f;

    using key_type = float;

    static constexpr float get_key(float f) { return f; }
  };

  const storage_type* impl;

  constexpr const storage_type* get_storage() const { return impl; }
};

struct data2 {
  struct storage_type {
    std::string_view str;

    using key_type = std::string_view;

    static constexpr std::string_view get_key(std::string_view str) {
      return str;
    }
  };

  const storage_type* impl;

  constexpr const storage_type* get_storage() const { return impl; }
};

class multi_data {
public:
  using impl_types = ely::variant<data0, data1, data2>;

  impl_types impl_;

  constexpr multi_data(data0 d0)
      : impl_(std::in_place_type<data0>, std::move(d0)) {}

  constexpr multi_data(data1 d1)
      : impl_(std::in_place_type<data1>, std::move(d1)) {}

  constexpr multi_data(data2 d2)
      : impl_(std::in_place_type<data2>, std::move(d2)) {}

  // template <typename... Args>
  // constexpr multi_data(Args&&... args) : impl_(static_cast<Args&&>(args)...)
  // {}

  constexpr impl_types get_variant() const { return impl_; }
};

// a simple use case for storageuniquer
template <typename T> bool test_uniquer() {

  static_assert(
      std::is_same_v<ely::get_storage_t<T>, typename T::storage_type>);

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

bool test_variant_uniquer() {
  auto uniq = ely::uniquer<multi_data>{};
  auto d0 = uniq.get_or_emplace<data0>(40);
  auto d1 = uniq.get_or_emplace<data1>(5.f);
  auto d2 = uniq.get_or_emplace<data2>("hello world");

  auto md0 = multi_data{d0};
  auto md1 = multi_data{d1};
  auto md2 = multi_data{d2};

  return true;
}

void uniquer() {
  assert(test_uniquer<data>());
  assert(test_variant_uniquer());
}

#ifndef NO_MAIN
int main() {
  uniquer();

  return 0;
}
#endif
