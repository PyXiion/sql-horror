cmake_minimum_required(VERSION 3.28)
project(test)

include(FetchContent)

set(CMAKE_CXX_STANDARD 26)

find_package(fmt REQUIRED)

add_executable(test src/main.cpp
    src/px/print.hpp
    src/px/sql/table.hpp
    src/px/sql/types.hpp
    src/px/templates/string_literal.hpp
    src/px/templates/derived_from.hpp
    src/px/sql/session.hpp
    src/px/sql/session.cpp
    src/px/sql/select.hpp
)
target_link_libraries(test fmt::fmt cppdb)