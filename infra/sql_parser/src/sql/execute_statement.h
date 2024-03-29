#ifndef SQLPARSER_EXECUTE_STATEMENT_H
#define SQLPARSER_EXECUTE_STATEMENT_H

#include "sql_statement.h"

namespace msql {

// Represents SQL Execute statements.
// Example: "EXECUTE ins_prep(100, "test", 2.3);"
struct ExecuteStatement : SQLStatement {
  ExecuteStatement();
  ~ExecuteStatement() override;

  char* name;
  std::vector<Expr*>* parameters;
};

}  // namespace msql

#endif
