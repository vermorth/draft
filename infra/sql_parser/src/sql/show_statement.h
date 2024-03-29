#ifndef SQLPARSER_SHOW_STATEMENT_H
#define SQLPARSER_SHOW_STATEMENT_H

#include "sql_statement.h"

// Note: Implementations of constructors and destructors can be found in
// statements.cpp.
namespace msql {

enum ShowType { kShowColumns, kShowTables };

// Represents SQL SHOW statements.
// Example "SHOW TABLES;"
struct ShowStatement : SQLStatement {
  ShowStatement(ShowType type);
  ~ShowStatement() override;

  ShowType type;
  char* schema;
  char* name;
};

}  // namespace msql
#endif
