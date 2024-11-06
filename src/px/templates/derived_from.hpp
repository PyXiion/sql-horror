//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_TEMPLATES_DERIVED_FROM_HPP
#define PX_TEST_SRC_PX_TEMPLATES_DERIVED_FROM_HPP

#include <type_traits>

namespace px {
  template<template <class ..., auto ...args> class base, class derived>
  struct is_base_of_template_impl {
    template<class ...Ts>
    static constexpr std::true_type test(const base<Ts ...> *);

    static constexpr std::false_type test(...);

    using type = decltype(test(std::declval<derived *>()));
  };

  template<template <class ...> class base, class derived>
  using is_base_of_template = typename is_base_of_template_impl<base, derived>::type;

  template<template <class ...> class base, class derived>
  constexpr bool is_base_of_template_v = is_base_of_template<base, derived>::value;
}

#endif //PX_TEST_SRC_PX_TEMPLATES_DERIVED_FROM_HPP
