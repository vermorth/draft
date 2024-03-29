#include "create_statement.h"
#include "select_statement.h"

namespace msql {

// CreateStatemnet
CreateStatement::CreateStatement(CreateType type)
    : SQLStatement(kStmtCreate),
      type(type),
      ifNotExists(false),
      filePath(nullptr),
      schema(nullptr),
      tableName(nullptr),
      indexName(nullptr),
      indexColumns(nullptr),
      columns(nullptr),
      tableConstraints(nullptr),
      viewColumns(nullptr),
      select(nullptr){};

CreateStatement::~CreateStatement() {
  free(filePath);
  free(schema);
  free(tableName);
  free(indexName);
  delete select;

  if (columns != nullptr) {
    for (ColumnDefinition* def : *columns) {
      delete def;
    }
    delete columns;
  }

  if (tableConstraints != nullptr) {
    for (TableConstraint* def : *tableConstraints) {
      delete def;
    }
    delete tableConstraints;
  }

  if (indexColumns != nullptr) {
    for (char* column : *indexColumns) {
      free(column);
    }
    delete indexColumns;
  }

  if (viewColumns != nullptr) {
    for (char* column : *viewColumns) {
      free(column);
    }
    delete viewColumns;
  }
}

void CreateStatement::setColumnDefsAndConstraints(
    std::vector<TableElement*>* tableElements) {
  columns = new std::vector<ColumnDefinition*>();
  tableConstraints = new std::vector<TableConstraint*>();

  for (auto tableElem : *tableElements) {
    if (auto* colDef = dynamic_cast<ColumnDefinition*>(tableElem)) {
      columns->emplace_back(colDef);
    } else if (auto* tableConstraint =
                   dynamic_cast<TableConstraint*>(tableElem)) {
      tableConstraints->emplace_back(tableConstraint);
    }
  }
}

}  // namespace msql
