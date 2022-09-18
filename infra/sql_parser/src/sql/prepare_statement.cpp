
#include "prepare_statement.h"

namespace msql {
// PrepareStatement
PrepareStatement::PrepareStatement()
    : SQLStatement(kStmtPrepare), name(nullptr), query(nullptr) {}

PrepareStatement::~PrepareStatement() {
  free(name);
  free(query);
}
}  // namespace msql
