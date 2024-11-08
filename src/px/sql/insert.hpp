//
// Created by pyxiion on 07.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_INSERT_HPP
#define PX_TEST_SRC_PX_SQL_INSERT_HPP

#include "table.hpp"

namespace px {
  template<TableConcept table_>
  struct insert_columns_ {

    table_ &table;
  };

  struct insert_table_arg_ {
    auto operator+(TableConcept auto &table) {
      return insert_columns_{table};
    }
  };
}

#define INSERT insert_table_arg_{} +
#define INTO

#endif //PX_TEST_SRC_PX_SQL_INSERT_HPP
