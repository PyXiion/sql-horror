//
// Created by pyxiion on 05.11.2024.
//
#include "session.hpp"
#include <cppdb/frontend.h>

struct px::session::impl_ {
  explicit impl_(const std::string &url)
      : session(url) {}

  cppdb::session session;
};

struct px::session_statement::impl_ {
  cppdb::statement statement;
};

struct px::session_result::impl_ {
  cppdb::result result;
};

px::session::session(const std::string &url) {
  impl = new impl_(url);
}

px::session::~session() {
  delete impl;
}

px::session_statement px::session::operator<<(const std::string &v) {
  auto stmt = impl->session << v;
  auto *stmt_impl = new px::session_statement::impl_{std::move(stmt)};
  return px::session_statement{stmt_impl};
}

px::session_statement::session_statement(px::session_statement::impl_ *impl)
    : impl(impl) {

}

px::session_statement::~session_statement() {
  delete impl;
}

px::session_statement &px::session_statement::operator<<(const std::string &v) {
  impl->statement << v;
  return *this;
}

px::session_statement &px::session_statement::operator<<(int v) {
  impl->statement << v;
  return *this;
}

px::session_statement &px::session_statement::operator<<(long long int v) {
  impl->statement << v;
  return *this;
}

px::session_statement &px::session_statement::operator<<(float v) {
  impl->statement << v;
  return *this;
}

px::session_statement &px::session_statement::operator<<(double v) {
  impl->statement << v;
  return *this;
}

px::session_result px::session_statement::operator<<(px::result_t) {
  cppdb::result r = impl->statement;
  return px::session_result{new px::session_result::impl_{std::move(r)}};
}

void px::session_statement::operator<<(px::exec_t) {
  impl->statement << cppdb::exec;
}

px::session_result::session_result(px::session_result::impl_ *impl)
    : impl(impl) {

}

px::session_result::~session_result() {
  delete impl;
}

bool px::session_result::next() {
  return impl->result.next();
}

px::session_result &px::session_result::operator>>(std::string &v) {
  impl->result >> v;
  return *this;
}

px::session_result &px::session_result::operator>>(int &v) {
  impl->result >> v;
  return *this;
}

px::session_result &px::session_result::operator>>(long long int &v) {
  impl->result >> v;
  return *this;
}

px::session_result &px::session_result::operator>>(float &v) {
  impl->result >> v;
  return *this;
}

px::session_result &px::session_result::operator>>(double &v) {
  impl->result >> v;
  return *this;
}

px::session_result &px::session_result::operator>>(short &v) {
  impl->result >> v;
  return *this;
}
