//
// Created by pyxiion on 08.11.2024.
//

#ifndef PX_TEST_SRC_PX_TEMPLATES_FALSE_HPP
#define PX_TEST_SRC_PX_TEMPLATES_FALSE_HPP

namespace px {
  template<class T>
  struct false_value_t {
    inline static constexpr bool value = false;
  };

  template<class T>
  inline static constexpr bool false_v = false_value_t<T>::value;
}

#endif //PX_TEST_SRC_PX_TEMPLATES_FALSE_HPP
