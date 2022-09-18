
#include "sql_helper.h"
#include <iostream>
#include <map>
#include <string>

namespace msql {

void printOperatorExpression(Expr* expr, uintmax_t numIndent);
void printAlias(Alias* alias, uintmax_t numIndent);

std::ostream& operator<<(std::ostream& os, const OperatorType& op);
std::ostream& operator<<(std::ostream& os, const DatetimeField& op);

std::string indent(uintmax_t numIndent) { return std::string(numIndent, '\t'); }
void inprint(int64_t val, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << "  " << std::endl;
}
void inprint(double val, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << std::endl;
}
void inprint(const char* val, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << std::endl;
}
void inprint(const char* val, const char* val2, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << "->" << val2 << std::endl;
}
void inprintC(char val, uintmax_t numIndent) {
  std::cout << indent(numIndent).c_str() << val << std::endl;
}
void inprint(const OperatorType& op, uintmax_t numIndent) {
  std::cout << indent(numIndent) << op << std::endl;
}
void inprint(const ColumnType& colType, uintmax_t numIndent) {
  std::cout << indent(numIndent) << colType << std::endl;
}
void inprint(const DatetimeField& colType, uintmax_t numIndent) {
  std::cout << indent(numIndent) << colType << std::endl;
}

void printTableRefInfo(TableRef* table, uintmax_t numIndent) {
  switch (table->type) {
    case kTableName:
      inprint(table->name, numIndent);
      if (table->schema) {
        inprint("Schema", numIndent + 1);
        inprint(table->schema, numIndent + 2);
      }
      break;
    case kTableSelect:
      PrintSelectStatementInfo(table->select, numIndent);
      break;
    case kTableJoin:
      inprint("Join Table", numIndent);
      inprint("Left", numIndent + 1);
      printTableRefInfo(table->join->left, numIndent + 2);
      inprint("Right", numIndent + 1);
      printTableRefInfo(table->join->right, numIndent + 2);
      inprint("Join Condition", numIndent + 1);
      PrintExpression(table->join->condition, numIndent + 2);
      break;
    case kTableCrossProduct:
      for (TableRef* tbl : *table->list) printTableRefInfo(tbl, numIndent);
      break;
  }

  if (table->alias) {
    printAlias(table->alias, numIndent);
  }
}

void printAlias(Alias* alias, uintmax_t numIndent) {
  inprint("Alias", numIndent + 1);
  inprint(alias->name, numIndent + 2);

  if (alias->columns) {
    for (char* column : *(alias->columns)) {
      inprint(column, numIndent + 3);
    }
  }
}

void printOperatorExpression(Expr* expr, uintmax_t numIndent) {
  if (expr == nullptr) {
    inprint("null", numIndent);
    return;
  }

  inprint(expr->opType, numIndent);

  PrintExpression(expr->expr, numIndent + 1);
  if (expr->expr2 != nullptr) {
    PrintExpression(expr->expr2, numIndent + 1);
  } else if (expr->exprList != nullptr) {
    for (Expr* e : *expr->exprList) PrintExpression(e, numIndent + 1);
  }
}

void PrintExpression(Expr* expr, uintmax_t numIndent) {
  if (!expr) return;
  switch (expr->type) {
    case kExprStar:
      inprint("*", numIndent);
      break;
    case kExprColumnRef:
      inprint(expr->name, numIndent);
      if (expr->table) {
        inprint("Table:", numIndent + 1);
        inprint(expr->table, numIndent + 2);
      }
      break;
    // case kExprTableColumnRef: inprint(expr->table, expr->name, numIndent);
    // break;
    case kExprLiteralFloat:
      inprint(expr->fval, numIndent);
      break;
    case kExprLiteralInt:
      inprint(expr->ival, numIndent);
      break;
    case kExprLiteralString:
      inprint(expr->name, numIndent);
      break;
    case kExprLiteralDate:
      inprint(expr->name, numIndent);
      break;
    case kExprLiteralNull:
      inprint("NULL", numIndent);
      break;
    case kExprLiteralInterval:
      inprint("INTERVAL", numIndent);
      inprint(expr->ival, numIndent + 1);
      inprint(expr->datetimeField, numIndent + 1);
      break;
    case kExprFunctionRef:
      inprint(expr->name, numIndent);
      for (Expr* e : *expr->exprList) PrintExpression(e, numIndent + 1);
      break;
    case kExprExtract:
      inprint("EXTRACT", numIndent);
      inprint(expr->datetimeField, numIndent + 1);
      PrintExpression(expr->expr, numIndent + 1);
      break;
    case kExprCast:
      inprint("CAST", numIndent);
      inprint(expr->columnType, numIndent + 1);
      PrintExpression(expr->expr, numIndent + 1);
      break;
    case kExprOperator:
      printOperatorExpression(expr, numIndent);
      break;
    case kExprSelect:
      PrintSelectStatementInfo(expr->select, numIndent);
      break;
    case kExprParameter:
      inprint(expr->ival, numIndent);
      break;
    case kExprArray:
      for (Expr* e : *expr->exprList) PrintExpression(e, numIndent + 1);
      break;
    case kExprArrayIndex:
      PrintExpression(expr->expr, numIndent + 1);
      inprint(expr->ival, numIndent);
      break;
    default:
      std::cerr << "Unrecognized expression type " << expr->type << std::endl;
      return;
  }
  if (expr->alias != nullptr) {
    inprint("Alias", numIndent + 1);
    inprint(expr->alias, numIndent + 2);
  }
}

void PrintOrderBy(const std::vector<OrderDescription*>* expr,
                  uintmax_t numIndent) {
  if (!expr) return;
  for (const auto& order_description : *expr) {
    PrintExpression(order_description->expr, numIndent);
    if (order_description->type == kOrderAsc) {
      inprint("ascending", numIndent);
    } else {
      inprint("descending", numIndent);
    }
  }
}

void PrintSelectStatementInfo(const SelectStatement* stmt,
                              uintmax_t numIndent) {
  inprint("SelectStatement", numIndent);
  inprint("Fields:", numIndent + 1);
  for (Expr* expr : *stmt->selectList) PrintExpression(expr, numIndent + 2);

  if (stmt->fromTable != nullptr) {
    inprint("Sources:", numIndent + 1);
    printTableRefInfo(stmt->fromTable, numIndent + 2);
  }

  if (stmt->whereClause != nullptr) {
    inprint("Search Conditions:", numIndent + 1);
    PrintExpression(stmt->whereClause, numIndent + 2);
  }

  if (stmt->groupBy != nullptr) {
    inprint("GroupBy:", numIndent + 1);
    for (Expr* expr : *stmt->groupBy->columns)
      PrintExpression(expr, numIndent + 2);
    if (stmt->groupBy->having != nullptr) {
      inprint("Having:", numIndent + 1);
      PrintExpression(stmt->groupBy->having, numIndent + 2);
    }
  }
  if (stmt->lockings != nullptr) {
    inprint("Lock Info:", numIndent + 1);
    for (LockingClause* lockingClause : *stmt->lockings) {
      inprint("Type", numIndent + 2);
      if (lockingClause->rowLockMode == RowLockMode::ForUpdate) {
        inprint("FOR UPDATE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForNoKeyUpdate) {
        inprint("FOR NO KEY UPDATE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForShare) {
        inprint("FOR SHARE", numIndent + 3);
      } else if (lockingClause->rowLockMode == RowLockMode::ForKeyShare) {
        inprint("FOR KEY SHARE", numIndent + 3);
      }
      if (lockingClause->tables != nullptr) {
        inprint("Target tables:", numIndent + 2);
        for (char* dtable : *lockingClause->tables) {
          inprint(dtable, numIndent + 3);
        }
      }
      if (lockingClause->rowLockWaitPolicy != RowLockWaitPolicy::None) {
        inprint("Waiting policy: ", numIndent + 2);
        if (lockingClause->rowLockWaitPolicy == RowLockWaitPolicy::NoWait)
          inprint("NOWAIT", numIndent + 3);
        else
          inprint("SKIP LOCKED", numIndent + 3);
      }
    }
  }

  if (stmt->setOperations != nullptr) {
    for (SetOperation* setOperation : *stmt->setOperations) {
      switch (setOperation->setType) {
        case SetType::kSetIntersect:
          inprint("Intersect:", numIndent + 1);
          break;
        case SetType::kSetUnion:
          inprint("Union:", numIndent + 1);
          break;
        case SetType::kSetExcept:
          inprint("Except:", numIndent + 1);
          break;
      }

      PrintSelectStatementInfo(setOperation->nestedSelectStatement,
                               numIndent + 2);

      if (setOperation->resultOrder != nullptr) {
        inprint("SetResultOrderBy:", numIndent + 1);
        PrintOrderBy(setOperation->resultOrder, numIndent + 2);
      }

      if (setOperation->resultLimit != nullptr) {
        if (setOperation->resultLimit->limit != nullptr) {
          inprint("SetResultLimit:", numIndent + 1);
          PrintExpression(setOperation->resultLimit->limit, numIndent + 2);
        }

        if (setOperation->resultLimit->offset != nullptr) {
          inprint("SetResultOffset:", numIndent + 1);
          PrintExpression(setOperation->resultLimit->offset, numIndent + 2);
        }
      }
    }
  }

  if (stmt->order != nullptr) {
    inprint("OrderBy:", numIndent + 1);
    PrintOrderBy(stmt->order, numIndent + 2);
  }

  if (stmt->limit != nullptr && stmt->limit->limit != nullptr) {
    inprint("Limit:", numIndent + 1);
    PrintExpression(stmt->limit->limit, numIndent + 2);
  }

  if (stmt->limit != nullptr && stmt->limit->offset != nullptr) {
    inprint("Offset:", numIndent + 1);
    PrintExpression(stmt->limit->offset, numIndent + 2);
  }
}

void PrintImportStatementInfo(const ImportStatement* stmt,
                              uintmax_t numIndent) {
  inprint("ImportStatement", numIndent);
  inprint(stmt->filePath, numIndent + 1);
  switch (stmt->type) {
    case ImportType::kImportCSV:
      inprint("CSV", numIndent + 1);
      break;
    case ImportType::kImportTbl:
      inprint("TBL", numIndent + 1);
      break;
    case ImportType::kImportBinary:
      inprint("BINARY", numIndent + 1);
      break;
    case ImportType::kImportAuto:
      inprint("AUTO", numIndent + 1);
      break;
  }
  inprint(stmt->tableName, numIndent + 1);
}

void PrintExportStatementInfo(const ExportStatement* stmt,
                              uintmax_t numIndent) {
  inprint("ExportStatement", numIndent);
  inprint(stmt->filePath, numIndent + 1);
  switch (stmt->type) {
    case ImportType::kImportCSV:
      inprint("CSV", numIndent + 1);
      break;
    case ImportType::kImportTbl:
      inprint("TBL", numIndent + 1);
      break;
    case ImportType::kImportBinary:
      inprint("BINARY", numIndent + 1);
      break;
    case ImportType::kImportAuto:
      inprint("AUTO", numIndent + 1);
      break;
  }
  inprint(stmt->tableName, numIndent + 1);
}

void PrintCreateStatementInfo(const CreateStatement* stmt,
                              uintmax_t numIndent) {
  inprint("CreateStatement", numIndent);
  inprint(stmt->tableName, numIndent + 1);
  if (stmt->filePath) inprint(stmt->filePath, numIndent + 1);
}

void PrintInsertStatementInfo(const InsertStatement* stmt,
                              uintmax_t numIndent) {
  inprint("InsertStatement", numIndent);
  inprint(stmt->tableName, numIndent + 1);
  if (stmt->columns != nullptr) {
    inprint("Columns", numIndent + 1);
    for (char* col_name : *stmt->columns) {
      inprint(col_name, numIndent + 2);
    }
  }
  switch (stmt->type) {
    case kInsertValues:
      inprint("Values", numIndent + 1);
      for (Expr* expr : *stmt->values) {
        PrintExpression(expr, numIndent + 2);
      }
      break;
    case kInsertSelect:
      PrintSelectStatementInfo(stmt->select, numIndent + 1);
      break;
  }
}

void PrintTransactionStatementInfo(const TransactionStatement* stmt,
                                   uintmax_t numIndent) {
  inprint("TransactionStatement", numIndent);
  switch (stmt->command) {
    case kBeginTransaction:
      inprint("BEGIN", numIndent + 1);
      break;
    case kCommitTransaction:
      inprint("COMMIT", numIndent + 1);
      break;
    case kRollbackTransaction:
      inprint("ROLLBACK", numIndent + 1);
      break;
  }
}

void PrintStatementInfo(const SQLStatement* stmt) {
  switch (stmt->type()) {
    case kStmtSelect:
      PrintSelectStatementInfo((const SelectStatement*)stmt, 0);
      break;
    case kStmtInsert:
      PrintInsertStatementInfo((const InsertStatement*)stmt, 0);
      break;
    case kStmtCreate:
      PrintCreateStatementInfo((const CreateStatement*)stmt, 0);
      break;
    case kStmtImport:
      PrintImportStatementInfo((const ImportStatement*)stmt, 0);
      break;
    case kStmtExport:
      PrintExportStatementInfo((const ExportStatement*)stmt, 0);
      break;
    case kStmtTransaction:
      PrintTransactionStatementInfo((const TransactionStatement*)stmt, 0);
      break;
    default:
      break;
  }
}

std::ostream& operator<<(std::ostream& os, const OperatorType& op) {
  static const std::map<const OperatorType, const std::string> operatorToToken =
      {{kOpNone, "None"},     {kOpBetween, "BETWEEN"},
       {kOpCase, "CASE"},     {kOpCaseListElement, "CASE LIST ELEMENT"},
       {kOpPlus, "+"},        {kOpMinus, "-"},
       {kOpAsterisk, "*"},    {kOpSlash, "/"},
       {kOpPercentage, "%"},  {kOpCaret, "^"},
       {kOpEquals, "="},      {kOpNotEquals, "!="},
       {kOpLess, "<"},        {kOpLessEq, "<="},
       {kOpGreater, ">"},     {kOpGreaterEq, ">="},
       {kOpLike, "LIKE"},     {kOpNotLike, "NOT LIKE"},
       {kOpILike, "ILIKE"},   {kOpAnd, "AND"},
       {kOpOr, "OR"},         {kOpIn, "IN"},
       {kOpConcat, "CONCAT"}, {kOpNot, "NOT"},
       {kOpUnaryMinus, "-"},  {kOpIsNull, "IS NULL"},
       {kOpExists, "EXISTS"}};

  const auto found = operatorToToken.find(op);
  if (found == operatorToToken.cend()) {
    return os << static_cast<uint64_t>(op);
  } else {
    return os << (*found).second;
  }
}

std::ostream& operator<<(std::ostream& os, const DatetimeField& datetime) {
  static const std::map<const DatetimeField, const std::string>
      operatorToToken = {
          {kDatetimeNone, "None"},     {kDatetimeSecond, "SECOND"},
          {kDatetimeMinute, "MINUTE"}, {kDatetimeHour, "HOUR"},
          {kDatetimeDay, "DAY"},       {kDatetimeMonth, "MONTH"},
          {kDatetimeYear, "YEAR"}};

  const auto found = operatorToToken.find(datetime);
  if (found == operatorToToken.cend()) {
    return os << static_cast<uint64_t>(datetime);
  } else {
    return os << (*found).second;
  }
}

}  // namespace msql
