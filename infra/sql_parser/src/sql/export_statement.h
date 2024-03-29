#ifndef SQLPARSER_EXPORT_STATEMENT_H
#define SQLPARSER_EXPORT_STATEMENT_H

#include "import_statement.h"
#include "sql_statement.h"

namespace msql {
// Represents SQL Export statements.
struct ExportStatement : SQLStatement {
  ExportStatement(ImportType type);
  ~ExportStatement() override;

  // ImportType is used for compatibility reasons
  ImportType type;
  char* filePath;
  char* schema;
  char* tableName;
};

}  // namespace msql

#endif
