#ifndef HYRISE_TRANSACTIONSTATEMENT_H
#define HYRISE_TRANSACTIONSTATEMENT_H

#include "sql_statement.h"

namespace msql {

// Represents SQL Transaction statements.
// Example: BEGIN TRANSACTION;
enum TransactionCommand {
  kBeginTransaction,
  kCommitTransaction,
  kRollbackTransaction
};

struct TransactionStatement : SQLStatement {
  TransactionStatement(TransactionCommand command);
  ~TransactionStatement() override;

  TransactionCommand command;
};

}  // namespace msql

#endif
