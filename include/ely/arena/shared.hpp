#pragma once

#include <memory>

namespace ely {
namespace arena {
class shared {
public:
  template <typename T> using ptr_type = std::shared_ptr<T>;

public:
  shared() = default;

  template <typename T, typename... Args>
  std::shared_ptr<T> make(Args&&... args) {
    return std::make_shared<T>(static_cast<Args&&>(args)...);
  }
};
} // namespace arena
} // namespace ely