#ifndef SQLPARSER_SQLPARSER_RESULT_H
#define SQLPARSER_SQLPARSER_RESULT_H

#include "sql/sql_statement.h"

namespace msql {
// Represents the result of the SQLParser.
// If parsing was successful it contains a list of SQLStatement.
class SQLParserResult {
 public:
  // Constructor / Destructor.

  // Initialize with empty statement list.
  SQLParserResult();

  // Initialize with a single statement.
  // Takes ownership of the statement.
  SQLParserResult(SQLStatement* stmt);

  // Copy constructor, deleted.
  SQLParserResult(const SQLParserResult&) = delete;
  SQLParserResult& operator=(const SQLParserResult&) = delete;

  // Move constructor.
  SQLParserResult(SQLParserResult&& moved);
  SQLParserResult& operator=(SQLParserResult&& moved);

  // Deletes all statements in the result.
  virtual ~SQLParserResult();

 public:
  // Getter / Setter.

  // Adds a statement to the result list of statements.
  // SQLParserResult takes ownership of the statement.
  void add_statement(SQLStatement* stmt);

  // Gets the SQL statement with the given index.
  const SQLStatement* statement(size_t index) const;

  // Gets the non const SQL statement with the given index.
  SQLStatement* mutable_statement(size_t index);

  // Get the list of all statements.
  const std::vector<SQLStatement*>& statements() const;

  // Returns the number of statements in the result.
  size_t statements_size() const;

  // Does not take ownership.
  void add_parameter(Expr* parameter);

  const std::vector<Expr*>& parameters();

  // Set whether parsing was successful.
  void set_is_valid(bool is_valid);

  // Returns true if parsing was successful.
  bool is_valid() const;

  // Set the details of the error, if available.
  // Takes ownership of error_msg.
  void set_error_details(char* error_msg, int error_line, int error_column);

  // Returns the error message, if an error occurred.
  const char* error_msg() const;

  // Returns the line number of the occurrance of the error in the query.
  int error_line() const;

  // Returns the column number of the occurrance of the error in the query.
  int error_column() const;

 public:
  // Other.

  // Deletes all statements and other data within the result.
  void Reset();

 private:
  // List of statements within the result.
  // Takes ownership of the statement.
  std::vector<SQLStatement*> statements_;

  // Does not have ownership of the parameter.
  std::vector<Expr*> parameters_;

  // Flag indicating the parsing was successful.
  bool is_valid_;

  // Error message, if an error occurred.
  char* error_msg_;

  // Line number of the occurrance of the error in the query.
  int error_line_;

  // Column number of the occurrance of the error in the query.
  int error_column_;
};

}  // namespace msql

#endif  // SQLPARSER_SQLPARSER_RESULT_H