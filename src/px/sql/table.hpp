//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_TABLE_HPP
#define PX_TEST_SRC_PX_SQL_TABLE_HPP
#include <tuple>
#include <sstream>
#include "types.hpp"
#include "../templates/string_literal.hpp"
#include "session.hpp"
#include "../templates/false.hpp"

namespace px {
  class session;
  struct table_base_ {};

  template<string_literal_ table_name, Column auto ...columns_>
  struct table_ : table_base_ {
    inline constexpr static auto name = table_name;
    inline constexpr static auto columns = std::make_tuple(columns_...);
    inline constexpr static auto npos = std::string::npos;
  public:

    inline constexpr operator std::string() const {
      std::stringstream ss;
      ss << "CREATE TABLE " << table_name.value << "(\n";

      int i = 1;
      ([&](Column auto column) {
        ss << "    ";
        ss << column.to_string();

        if (i++ != sizeof...(columns_))
          ss << ',';
        ss << '\n';
      }(columns_), ...);

      ss << ')';
      return ss.str();
    }

    inline static consteval std::size_t get_column_index_by_name(StringLiteral auto const &name_) {
      std::size_t r = npos;
      std::size_t i = 0;
      ([&](auto &&column) {
        if (column.name == name_)
          r = i;
        i++;
      }(columns_), ...);

      // TODO: better error
      if (r == npos) {
        throw std::invalid_argument("Column not found");
      }

      return r;
    }

    using value_types = std::tuple<typename std::decay_t<decltype(columns_.type)>::value_type...>;

    template<string_literal_ ...names_>
    using value_types_for = std::tuple<decltype(std::get<get_column_index_by_name(names_)>(columns))...>;

    inline static auto connect_to(session &session) {
      return table_{session};
    }

    inline std::string to_string() {
      std::stringstream ss;
      ss << "CREATE TABLE " << table_name.value << " (\n";
      int i = 1;
      ([&](Column auto column) {
        ss << "    " << column.name.value << " " << column.type.to_string();
        if (i++ != sizeof...(columns_))
          ss << ", \n";
      }(columns_), ...);
      ss << "\n)";
      return ss.str();
    }

    template<class T>
    inline consteval static bool is_allowed_column_name_type() {
      bool allowed = false;
      ([&allowed](auto &allowed_value) {
        using allowed_value_type = std::decay_t<decltype(allowed_value)>;
        if constexpr (std::convertible_to<T, allowed_value_type>) {
          allowed = true;
        }
      }(columns_.name), ...);
      return allowed;
    }

    inline consteval static bool is_allowed_column_name(StringLiteral auto column_name) {
      bool allowed = false;
      ([&allowed, &column_name](auto &allowed_value) {
        if (allowed_value == column_name) {
          allowed = true;
        }
      }(columns_.name), ...);
      return allowed;
    }
  };

  template<class T>
  concept TableConcept = std::derived_from<T, table_base_>;

  template<TableConcept table_, string_literal_ name>
  inline constexpr auto column_by_name = std::get<table_::get_column_index_by_name(name)>(table_::columns);


  template<TableConcept table_>
  struct table_fabric_ {
    auto operator +(session &session) const {
      return table_::connect_to(session);
    }
  };

  struct create_t{
    void operator+(auto &table) {
      table.session << table.to_string() << exec;
    }
  };
}

#define TABLE px::table_<
#define END_TABLE >{}
#define CREATE px::create_t{} +

#endif //PX_TEST_SRC_PX_SQL_TABLE_HPP
