
#include "sql_parser_result.h"
#include <algorithm>

namespace msql {

SQLParserResult::SQLParserResult() : is_valid_(false), error_msg_(nullptr){};

SQLParserResult::SQLParserResult(SQLStatement* stmt)
    : is_valid_(false), error_msg_(nullptr) {
  add_statement(stmt);
};

// Move constructor.
SQLParserResult::SQLParserResult(SQLParserResult&& moved) {
  *this = std::forward<SQLParserResult>(moved);
}

SQLParserResult& SQLParserResult::operator=(SQLParserResult&& moved) {
  is_valid_ = moved.is_valid_;
  error_msg_ = moved.error_msg_;
  statements_ = std::move(moved.statements_);

  moved.error_msg_ = nullptr;
  moved.Reset();
  return *this;
}

SQLParserResult::~SQLParserResult() { Reset(); }

void SQLParserResult::add_statement(SQLStatement* stmt) {
  statements_.push_back(stmt);
}

const SQLStatement* SQLParserResult::statement(size_t index) const {
  return statements_[index];
}

SQLStatement* SQLParserResult::mutable_statement(size_t index) {
  return statements_[index];
}

const std::vector<SQLStatement*>& SQLParserResult::statements() const {
  return statements_;
}

size_t SQLParserResult::statements_size() const { return statements_.size(); }

void SQLParserResult::add_parameter(Expr* parameter) {
  parameters_.push_back(parameter);
  std::sort(parameters_.begin(), parameters_.end(),
            [](const Expr* a, const Expr* b) { return a->ival < b->ival; });
}

const std::vector<Expr*>& SQLParserResult::parameters() { return parameters_; }

void SQLParserResult::set_is_valid(bool is_valid) { is_valid_ = is_valid; }

bool SQLParserResult::is_valid() const { return is_valid_; }

void SQLParserResult::set_error_details(char* error_msg, int error_line,
                                        int error_column) {
  error_msg_ = error_msg;
  error_line_ = error_line;
  error_column_ = error_column;
}

const char* SQLParserResult::error_msg() const { return error_msg_; }

int SQLParserResult::error_line() const { return error_line_; }

int SQLParserResult::error_column() const { return error_column_; }

void SQLParserResult::Reset() {
  for (SQLStatement* statement : statements_) {
    if (statement != nullptr) {
      delete statement;
      statement = nullptr;
    }
  }
  statements_.clear();

  is_valid_ = false;

  free(error_msg_);
  error_msg_ = nullptr;
  error_line_ = -1;
  error_column_ = -1;
}

}  // namespace msql
