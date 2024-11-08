//
// Created by pyxiion on 05.11.2024.
//

#ifndef PX_TEST_SRC_PX_SQL_SESSION_HPP
#define PX_TEST_SRC_PX_SQL_SESSION_HPP

#include <string>

namespace px {
  struct session_result {
    friend class session_statement;
    session_result(session_result &&other) noexcept : impl(other.impl) { other.impl = nullptr; }
    ~session_result();

    bool next();

    session_result &operator>>(std::string &v);

    session_result &operator>>(short &v);

    session_result &operator>>(int &v);

    session_result &operator>>(long long &v);

    session_result &operator>>(float &v);

    session_result &operator>>(double &v);

    template<class T>
    T get() {
      T v;
      *this >> v;
      return v;
    }

  private:
    struct impl_;

    explicit session_result(impl_ *impl);


    impl_ *impl;
  };

  struct result_t {};
  struct exec_t {};

  inline constexpr static auto result = result_t{};
  inline constexpr static auto exec = exec_t{};

  class session_statement {
  public:
    friend class session;
    session_statement(session_statement &&other) noexcept : impl(other.impl) { other.impl = nullptr; }
    ~session_statement();

    session_statement &operator<<(const std::string &v);

    session_statement &operator<<(int v);

    session_statement &operator<<(long long v);

    session_statement &operator<<(float v);

    session_statement &operator<<(double v);

    session_result operator<<(result_t);

    void operator<<(exec_t);

  private:
    struct impl_;

    explicit session_statement(impl_ *impl);


    impl_ *impl;
  };

  class session {
    struct impl_;
  public:
    explicit session(const std::string &url);
    session(session &&other) noexcept : impl(other.impl) { other.impl = nullptr; }

    ~session();

    session_statement operator<<(const std::string &v);

    auto execute(const auto &executable) requires requires { executable.execute(*this); } {
      return executable.execute(*this);
    }

  private:
    impl_ *impl;
  };
}

#endif //PX_TEST_SRC_PX_SQL_SESSION_HPP
