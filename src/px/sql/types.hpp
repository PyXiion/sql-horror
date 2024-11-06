//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_TYPES_TYPES_HPP
#define PX_TEST_SRC_PX_SQL_TYPES_TYPES_HPP

#include <tuple>
#include <string>
#include <fmt/format.h>
#include "../templates/string_literal.hpp"
#include "../templates/derived_from.hpp"

namespace px {
  struct basic_type_ {};

  template<class ValueType, string_literal_ name_>
  struct type_ : basic_type_ {
    inline static constexpr const char *name = name_.value;
    using value_type = ValueType;

    [[nodiscard]] virtual std::string to_string() const {
      return fmt::format("{}", name_.value);
    }
  };

  template<class T>
  concept TypeConcept = std::derived_from<std::decay_t<T>, basic_type_>;

  // Text
  template<class ValueType, string_literal_ name_>
  struct sized_type_
      : type_<ValueType, name_> {
    std::size_t size;

    explicit constexpr sized_type_(std::size_t size)
        : size(size) {}

    [[nodiscard]] std::string to_string() const override {
      return fmt::format("{}({})", name_.value, size);
    }
  };

  // Text
  using CHAR = sized_type_<std::string, "CHAR">;
  using VARCHAR = sized_type_<std::string, "VARCHAR">;

  // Numeric
  constexpr inline auto SMALLINT = type_<short, "SMALLINT">{};
  constexpr inline auto INTEGER = type_<int, "INTEGER">{};

  // Flags
  enum struct Constrains {
    PRIMARY_KEY, NOT_NULL, UNIQUE
  };

  template<Constrains constraint>
  struct constraint_ {};

  inline constexpr auto PRIMARY_KEY = constraint_<Constrains::PRIMARY_KEY>{};
  inline constexpr auto NOT_NULL = constraint_<Constrains::NOT_NULL>{};
  inline constexpr auto UNIQUE = constraint_<Constrains::UNIQUE>{};

  template<TypeConcept Type, std::size_t N>
  struct column_ {
    Type type;
    string_literal_<N> name;
    bool primary_key = false;
    bool not_null = false;
    bool unique = false;

    constexpr column_(Type &&type, string_literal_<N> &&literal)
        : type(std::forward<Type>(type)),
          name(std::move(literal)) {}

    template<Constrains constraint>
    constexpr column_ &operator-(const constraint_<constraint> &) {
      switch (constraint) {
        case Constrains::PRIMARY_KEY:
          primary_key = true;
          break;
        case Constrains::NOT_NULL:
          not_null = true;
          break;
        case Constrains::UNIQUE:
          unique = true;
          break;
      }
      return *this;
    }
  };

  template<class T>
  concept Column = requires(T column) {
    { column.type } -> TypeConcept;
    { column.name } -> StringLiteral;

    { column.primary_key } -> std::convertible_to<bool>;
    { column.not_null } -> std::convertible_to<bool>;
    { column.unique } -> std::convertible_to<bool>;
  };

  template<std::size_t N, TypeConcept Type>
  consteval auto operator-(const char (&column_name)[N], Type &&type) -> column_<Type, N> {
    return {std::forward<Type>(type), column_name};
  }
}

#endif //PX_TEST_SRC_PX_SQL_TYPES_TYPES_HPP
