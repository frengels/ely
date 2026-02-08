#include <random>
#include <string>

namespace {
int parens_level = 0;

std::string gen_ident(std::mt19937& rng) {
  auto length = std::uniform_int_distribution<std::size_t>(4, 64)(rng);
  std::string ident;
  ident.reserve(length);
  std::uniform_int_distribution<char> dist('a', 'z');
  for (std::size_t i = 0; i < length; ++i) {
    ident += dist(rng);
  }
  return ident + " ";
}

std::string gen_lparens(std::mt19937&) {
  ++parens_level;
  return "(";
}

std::string gen_rparens(std::mt19937& rng) {
  if (parens_level > 0) {
    --parens_level;
    return ")";
  } else {
    return gen_lparens(rng);
  }
}

std::string gen_num_no_ws(std::mt19937& rng) {
  auto length = std::uniform_int_distribution<std::size_t>(1, 16)(rng);
  std::string num;
  num.reserve(length);
  std::uniform_int_distribution<char> dist('0', '9');
  for (std::size_t i = 0; i < length; ++i) {
    num += dist(rng);
  }
  return num;
}

std::string gen_num(std::mt19937& rng) { return gen_num_no_ws(rng) + " "; }

std::string gen_decimal(std::mt19937& rng) {
  return gen_num_no_ws(rng) + "." + gen_num_no_ws(rng) + " ";
}

std::string gen_whitespace(std::mt19937& rng) {
  std::uniform_int_distribution<char> dist(0, 10);
  switch (dist(rng)) {
  case 0:
    return "\t";
  case 1:
    return "\n";
  }
  return " ";
}

std::string close_parens() {
  std::string res;
  while (parens_level-- > 0) {
    res += ")";
  }
  return res;
}

constexpr std::add_pointer_t<std::string(std::mt19937&)> actions[] = {
    &gen_ident, &gen_lparens, &gen_rparens,
    &gen_num,   &gen_decimal, &gen_whitespace,
};
} // namespace

static inline std::string gen_src(std::size_t len, std::size_t seed = 42) {
  std::mt19937 rng(seed);
  std::uniform_int_distribution<std::size_t> dist(0, std::size(actions) - 1);

  std::string output;
  output.reserve(len);

  while (output.size() < len) {
    output += actions[dist(rng)](rng);
  }

  output += close_parens();
  output += '\0';
  return output;
}
