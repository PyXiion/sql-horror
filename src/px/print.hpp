//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_PRINT_HPP
#define PX_TEST_SRC_PX_PRINT_HPP

#include <string_view>
#include <fmt/format.h>
#include <tuple>

namespace px {
  template<bool new_line, class ...T>
  struct print_ {
    std::tuple<T...> args;
    std::string_view format;
    bool print = true;

    constexpr ~print_() {
      if (print) {
        std::apply([this](auto &&...x) { fmt::print(fmt::runtime(format), std::forward<decltype(x)>(x)...); }, args);
        if (new_line)
          puts("");
      }
    }

    template<class Arg>
    auto operator,(Arg &&arg) {
      print = false;
      if constexpr (sizeof...(T)) {
        auto new_args = std::tuple_cat(std::move(args), std::make_tuple<Arg>(std::forward<Arg>(arg)));
        return print_<new_line, T..., std::decay_t<Arg>>{
            std::move(new_args),
            format
        };
      } else {
        return print_<new_line, Arg>{{arg}, format};
      }
    };
  };

  template<bool new_line>
  struct print_first_arg_ {
    print_<new_line> operator,(std::string_view format) {
      return {{}, format};
    }
  };
}

#define print px::print_first_arg_<false>{},
#define println px::print_first_arg_<true>{},

#endif //PX_TEST_SRC_PX_PRINT_HPP
