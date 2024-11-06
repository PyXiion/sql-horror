//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_TEMPLATES_STRING_LITERAL_HPP
#define PX_TEST_SRC_PX_TEMPLATES_STRING_LITERAL_HPP

#include <cstddef>
#include <algorithm>

namespace px {
  template<std::size_t N>
  struct string_literal_ {
    constexpr string_literal_() = default;

    constexpr string_literal_(const char (&str)[N]) {
      std::copy_n(str, N, value);
    }

    char value[N]{};
    const std::size_t length = N;
  };

  template<typename T>
  concept StringLiteral = requires(std::decay_t<T> t) {
    { t.value } -> std::convertible_to<const char*>;
    { t.length } -> std::convertible_to<std::size_t>;
  };

  consteval bool operator ==(StringLiteral auto const &l, StringLiteral auto const &r)  {
    if (l.length != r.length) return false;
    return std::equal(l.value, l.value + l.length, r.value);
  }

  constexpr bool operator +(StringLiteral auto l, StringLiteral auto r) {
    string_literal_<l.length + r.length> result;
    auto it = std::copy_n(std::begin(l.value), l.length, std::begin(result.value));
    std::copy_n(std::begin(r.value), r.length, it);
    return result;
  }
}

template<std::size_t N>
struct fmt::formatter<px::string_literal_<N>> : formatter<string_view> {
  constexpr auto format(const px::string_literal_<N> &l, format_context &ctx) const {
    return formatter<string_view>::format(l.value, ctx);
  }
};

#endif //PX_TEST_SRC_PX_TEMPLATES_STRING_LITERAL_HPP
