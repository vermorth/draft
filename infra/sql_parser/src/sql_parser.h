#ifndef SQLPARSER_SQLPARSER_H
#define SQLPARSER_SQLPARSER_H

#include "sql/statements.h"
#include "sql_parser_result.h"

namespace msql {

// Static methods used to parse SQL strings.
namespace SQLParser {
// Parses a given constant character SQL string into the result object.
// Returns true if the lexer and parser could run without internal errors.
// This does NOT mean that the SQL string was valid SQL. To check that
// you need to check result->is_valid();
bool Parse(const std::string& sql, SQLParserResult* result);

// Run tokenization on the given string and store the tokens in the output
// vector.
bool Tokenize(const std::string& sql, std::vector<int16_t>* tokens);

}  // namespace SQLParser
}  // namespace msql

#endif