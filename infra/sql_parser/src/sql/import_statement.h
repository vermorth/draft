#ifndef SQLPARSER_IMPORT_STATEMENT_H
#define SQLPARSER_IMPORT_STATEMENT_H

#include "sql_statement.h"

namespace msql {
enum ImportType {
  kImportCSV,
  kImportTbl,  // Hyrise file format
  kImportBinary,
  kImportAuto
};

// Represents SQL Import statements.
struct ImportStatement : SQLStatement {
  ImportStatement(ImportType type);
  ~ImportStatement() override;

  ImportType type;
  char* filePath;
  char* schema;
  char* tableName;
};

}  // namespace msql

#endif
