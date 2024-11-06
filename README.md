## sql-horror (WIP)

A silly SQL ORM with SQL-like syntax.

Usage example:
```c++
#include "px/sql/table.hpp"
#include "px/print.hpp"
#include "px/sql/session.hpp"
#include "px/sql/select.hpp"


int main() {
  px::session session("sqlite3:db=database.db");
  
  auto user_table =
    TABLE "users",
        "id"        - px::INTEGER     - px::NOT_NULL - px::PRIMARY_KEY,
        "username"  - px::CHAR(16)    - px::NOT_NULL - px::UNIQUE,
        "age"       - px::SMALLINT    - px::NOT_NULL,
        "bio"       - px::VARCHAR(128)
    CONNECT_TO session;
  
  CREATE user_table;
  
  println "User:";
  for (const auto &&[
          username,
          bio,
          id,
          age] : SELECT "username", "bio", "id", "age" FROM user_table) {
    println "User @{} ({}), age: {}, bio: {}", username, id, age, bio;
  }
  
  for (const auto &&[_, bio, id] : SELECT "age", "bio", "id" FROM user_table) {
    println "Id: {}, bio: {}", id, bio;
  }
}
```