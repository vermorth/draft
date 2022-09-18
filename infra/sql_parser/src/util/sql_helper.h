#ifndef SQLPARSER_SQLHELPER_H
#define SQLPARSER_SQLHELPER_H

#include "../sql/statements.h"

namespace msql {

// Prints a summary of the given SQLStatement.
void PrintStatementInfo(const SQLStatement* stmt);

// Prints a summary of the given SelectStatement with the given indentation.
void PrintSelectStatementInfo(const SelectStatement* stmt,
                              uintmax_t num_indent);

// Prints a summary of the given ImportStatement with the given indentation.
void PrintImportStatementInfo(const ImportStatement* stmt,
                              uintmax_t num_indent);

// Prints a summary of the given CopyStatement with the given indentation.
void PrintExportStatementInfo(const ExportStatement* stmt,
                              uintmax_t num_indent);

// Prints a summary of the given InsertStatement with the given indentation.
void PrintInsertStatementInfo(const InsertStatement* stmt,
                              uintmax_t num_indent);

// Prints a summary of the given CreateStatement with the given indentation.
void PrintCreateStatementInfo(const CreateStatement* stmt,
                              uintmax_t num_indent);

// Prints a summary of the given TransactionStatement with the given
// indentation.
void PrintTransactionStatementInfo(const TransactionStatement* stmt,
                                   uintmax_t num_indent);

// Prints a summary of the given Expression with the given indentation.
void PrintExpression(Expr* expr, uintmax_t num_indent);

// Prints an ORDER BY clause
void PrintOrderBy(const std::vector<OrderDescription*>* expr,
                  uintmax_t num_indent);

}  // namespace msql

#endif
