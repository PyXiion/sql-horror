//
// Created by pyxiion on 06.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_SELECT_HPP
#define PX_TEST_SRC_PX_SQL_SELECT_HPP

#include <concepts>
#include <variant>
#include <vector>
#include "../templates/string_literal.hpp"
#include "table.hpp"

namespace px {
  template<TableConcept table_, Column auto ...selected_columns_>
  struct select_;
  template<TableConcept table_>
  struct where_;

  struct where_fabric_ {};

  enum class operators {
    LESS, LESS_EQUAL,
    GREATER, GREATER_EQUAL,
    EQUALS,
    LIKE,

    AND, OR
  };

  struct and_{};
  struct or_{};

  struct condition_ {
    const std::string_view name;
    operators op{};
    std::variant<std::nullptr_t, short, int, long long> value;

    [[nodiscard]] inline std::string to_string() const {
      if (op == operators::AND) {
        return "AND";
      } else if (op == operators::OR) {
        return "OR";
      }

      std::stringstream ss;
      ss << name << " ";
      switch (op) {
        case operators::EQUALS: ss << "=";
          break;
        case operators::GREATER_EQUAL: ss << ">=";
          break;
        case operators::GREATER: ss << ">";
          break;
        case operators::LESS_EQUAL: ss << "<=";
          break;
        case operators::LESS: ss << "<";
          break;
        case operators::LIKE: ss << "LIKE";
          break;
      }
      ss << " ";
      std::visit([&ss](const auto &v) { ss << v; }, value);
      return ss.str();
    }
  };

  template<TableConcept table_, Column auto ...selected_columns_>
  struct select_ {
    using value_types = std::tuple<typename std::decay_t<decltype(selected_columns_.type)>::value_type...>;

    struct result_ {
      struct iterator_ {
        explicit iterator_(session_result &&result)
            : result(std::move(result)) {
          has_more = this->result.next();
        }

        value_types operator*() {
          return std::make_tuple(result.get<typename std::decay_t<decltype(selected_columns_.type)>::value_type>()...);
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
        return iterator_{session << query << result};
      }

      inline constexpr std::default_sentinel_t end() const { return std::default_sentinel; }

      std::string query;
      px::session &session;
    };

    inline auto execute(session &session) const {
      return result_ {to_string(), session};
    }

    [[nodiscard]] std::string to_string() const {
      std::stringstream ss;
      ss << "SELECT ";
      int i = 1;
      ([&](Column auto &column) {
        ss << column.name.value;
        if (i++ != sizeof...(selected_columns_))
          ss << ", ";
      }(selected_columns_), ...);
      ss << " FROM " << table_::name.value;

      if (not conditions.empty()) {
        ss << " WHERE ";
        int i = 1;
        for (const auto &condition : conditions) {
          ss << condition.to_string();
        }
      }

      return ss.str();
    }

    constexpr auto operator%(where_fabric_);
//    constexpr auto operator%(and_);

    constexpr auto append_condition(condition_ &&cond) {
      conditions.emplace_back(std::move(cond));
      return *this;
    }

    const table_ &table;
    std::vector<condition_> conditions;
  };

  template<string_literal_ ...columns_>
  struct select_fabric_ {

    inline constexpr auto operator%(TableConcept auto const &table) {
      using table_ = typename std::decay_t<decltype(table)>;
      return px::select_<table_, column_by_name<table_, columns_>...>{table};
    }
  };


  template<TableConcept table_, Column auto ...columns>
  struct column_name_acceptor_fabric_;

  template<TableConcept table_, std::size_t N, Column auto ...columns>
  struct column_name_acceptor_ {
    inline constexpr explicit column_name_acceptor_(const select_<table_, columns...> select, string_literal_<N> l)
        : select(select),
          name(l) {
      if (not table_::is_allowed_column_name(l))
        throw std::invalid_argument("Unknown column name.");
    }

    inline constexpr auto operator<(std::integral auto v) {
      return select.append_condition({name.value, operators::LESS, v});
    }

    inline constexpr auto operator>(std::integral auto v) {
      return select.append_condition({name.value, operators::GREATER, v});
    }

    inline constexpr auto operator<=(std::integral auto v) {
      return select.append_condition({name.value, operators::LESS_EQUAL, v});
    }

    inline constexpr auto operator>=(std::integral auto v) {
      return select.append_condition({name.value, operators::GREATER_EQUAL, v});
    }

    inline constexpr auto operator=(std::integral auto v) {
      return select.append_condition({name.value, operators::EQUALS, v});
    }

    inline constexpr auto operator<=>(std::integral auto v) {
      return select.append_condition({name.value, operators::LIKE, v});
    }

    // TODO: save it somewhere
    select_<table_, columns...> select;
    string_literal_<N> name;
  };

  template<TableConcept table_, Column auto ...columns>
  struct column_name_acceptor_fabric_ {
    template<std::size_t N>
    constexpr auto operator%(const char (&name)[N]) {
      return column_name_acceptor_<table_, N, columns...>{select, name};
    }

    const select_<table_, columns...> select;
  };

  template<TableConcept table_, Column auto... columns>
  constexpr auto select_<table_, columns...>::operator%(where_fabric_) {
    return column_name_acceptor_fabric_<table_, columns...>{*this};
  }

//  template<TableConcept table_, Column auto... columns>
//  constexpr auto select_<table_, columns...>::operator%(and_) {
//    return column_name_acceptor_fabric_<table_, columns...>{append_condition({"", operators::AND, nullptr})}
//  }
}

#define SELECT px::select_fabric_<
#define FROM >{} %
#define WHERE % px::where_fabric_{} %
#define LIKE <=>

#endif //PX_TEST_SRC_PX_SQL_SELECT_HPP
