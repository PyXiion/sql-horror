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
  // Basic SQL-type class. Just for concepts
  struct basic_type_ {};

  template<class T>
  concept TypeConcept = std::derived_from<std::decay_t<T>, basic_type_>;

  /**
   * Basic SQL-type class.
   * \tparam ValueType The C++ type it represents.
   * \tparam name_ Its name in SQL.
   */
  template<class ValueType, string_literal_ name_>
  struct type_ : basic_type_ {
    inline static constexpr const char *name = name_.value;
    using value_type = ValueType;

    [[nodiscard]] virtual std::string to_string() const {
      return fmt::format("{}", name_.value);
    }
  };

  /**
   * SQL-type which is taking a size argument and formats into NAME(size)
   */
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

  /**
   * SQL text type with fixed size.
   */
  using CHAR = sized_type_<std::string, "CHAR">;
  /**
   * SQL text type with limited size.
   */
  using VARCHAR = sized_type_<std::string, "VARCHAR">;

  // Numeric
  constexpr inline auto SMALLINT = type_<short, "SMALLINT">{};
  constexpr inline auto INTEGER = type_<int, "INTEGER">{};

  enum struct Constrains {
    PRIMARY_KEY = 1 << 0,
    NOT_NULL = 1 << 2,
    UNIQUE = 1 << 3,
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
    int constraints = 0;

    constexpr column_(Type &&type, string_literal_<N> &&literal)
        : type(std::forward<Type>(type)),
          name(std::move(literal)) {}

    template<Constrains constraint>
    constexpr column_ &operator-(const constraint_<constraint> &) {
      constraints |= static_cast<int>(constraint);
      return *this;
    }

    [[nodiscard]] std::string to_string() const {
      std::stringstream ss;
      ss << name << " " << Type::name.value;
      if (constraints & static_cast<int>(Constrains::PRIMARY_KEY))
        ss << " PRIMARY KEY";
      if (constraints & static_cast<int>(Constrains::NOT_NULL))
        ss << " NOT NULL";
      if (constraints & static_cast<int>(Constrains::UNIQUE))
        ss << " UNIQUE";

      return ss.str();
    }
  };

  template<class T>
  concept Column = requires(T column) {
    { column.type } -> TypeConcept;
    { column.name } -> StringLiteral;

    { column.constraints } -> std::convertible_to<int>;
  };

  template<std::size_t N, TypeConcept Type>
  consteval auto operator-(const char (&column_name)[N], Type &&type) -> column_<Type, N> {
    return {std::forward<Type>(type), column_name};
  }
}

#endif //PX_TEST_SRC_PX_SQL_TYPES_TYPES_HPP
