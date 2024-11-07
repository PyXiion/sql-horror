//
// Created by pyxiion on 06.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_SELECT_HPP
#define PX_TEST_SRC_PX_SQL_SELECT_HPP
#include <concepts>
#include "../templates/string_literal.hpp"
#include "table.hpp"

namespace px {
  template<TableConcept table_, Column auto ...columns_>
  struct select_ {
    using value_types = std::tuple<typename std::decay_t<decltype(columns_.type)>::value_type...>;

    struct iterator_ {
      explicit iterator_(session_result &&result)
          : result(std::move(result)) {
        has_more = this->result.next();
      }

      value_types operator*() {
        return std::make_tuple(result.get<typename std::decay_t<decltype(columns_.type)>::value_type>()...);
      }

      iterator_ &operator++() {
        has_more = result.next();
        return *this;
      }

      bool operator==(std::default_sentinel_t) const {
        return not has_more;
      }

      session_result result;
      bool has_more;
    };

    inline iterator_ begin() const {
      return iterator_{table.session << to_string() << result};
    }

    inline constexpr std::default_sentinel_t end() const { return std::default_sentinel; }

    [[nodiscard]] std::string to_string() const {
      std::stringstream ss;
      ss << "SELECT ";
      int i = 1;
      ([&](Column auto &column) {
        ss << column.name.value;
        if (i++ != sizeof...(columns_))
          ss << ", ";
      }(columns_), ...);
      ss << " FROM " << table_::name.value;
      return ss.str();
    }

    const table_ &table;
  };

  template<string_literal_ ...columns_>
  struct select_fabric_ {

    inline auto operator%(TableConcept auto const &table) {
      using table_ = typename std::decay_t<decltype(table)>;
      return px::select_<table_, column_by_name<table_, columns_>...>{table};
    }
  };
}

#define SELECT px::select_fabric_<
#define FROM >{} %

#endif //PX_TEST_SRC_PX_SQL_SELECT_HPP
