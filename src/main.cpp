#include "px/sql/table.hpp"
#include "px/print.hpp"
#include "px/sql/session.hpp"
#include "px/sql/select.hpp"

constexpr auto user_table =
    TABLE "users",
          "id" - px::INTEGER - px::NOT_NULL - px::PRIMARY_KEY,
          "username" - px::CHAR(16) - px::NOT_NULL - px::UNIQUE,
          "age" - px::SMALLINT - px::NOT_NULL,
          "bio" - px::VARCHAR(128)
    END_TABLE;

int main() {
  px::session session("sqlite3:db=database.db");

  for (const auto &&[username]
      : session.execute(SELECT "username" FROM user_table WHERE "id" < 5)) {
    println "User @{}", username;
  }

  auto statement = SELECT "id", "bio" FROM user_table WHERE "age" > 25;
  for (const auto &&[
        id,
        bio] : session.execute(statement)) {
    println "User #{} : {}", id, bio;
  }


  auto causes_error = SELECT "id", "bio" FROM user_table WHERE "agse" > 25;
}