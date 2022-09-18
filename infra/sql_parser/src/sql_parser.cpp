
#include "sql_parser.h"
#include <stdio.h>
#include <string>
#include "parser/bison_parser.h"
#include "parser/flex_lexer.h"

namespace msql {

// static
bool SQLParser::Parse(const std::string& sql, SQLParserResult* result) {
  yyscan_t scanner;
  YY_BUFFER_STATE state;

  if (msql_lex_init(&scanner)) {
    // Couldn't initialize the lexer.
    fprintf(stderr, "SQLParser: Error when initializing lexer!\n");
    return false;
  }
  const char* text = sql.c_str();
  state = msql__scan_string(text, scanner);

  // Parse the tokens.
  // If parsing fails, the result will contain an error object.
  int ret = msql_parse(result, scanner);
  bool success = (ret == 0);
  result->set_is_valid(success);

  msql__delete_buffer(state, scanner);
  msql_lex_destroy(scanner);

  return true;
}

// static
bool SQLParser::Tokenize(const std::string& sql, std::vector<int16_t>* tokens) {
  // Initialize the scanner.
  yyscan_t scanner;
  if (msql_lex_init(&scanner)) {
    fprintf(stderr, "SQLParser: Error when initializing lexer!\n");
    return false;
  }

  YY_BUFFER_STATE state;
  state = msql__scan_string(sql.c_str(), scanner);

  YYSTYPE yylval;
  YYLTYPE yylloc;

  // Step through the string until EOF is read.
  // Note: msql_lex returns int, but we know that its range is within 16 bit.
  int16_t token = msql_lex(&yylval, &yylloc, scanner);
  while (token != 0) {
    tokens->push_back(token);
    token = msql_lex(&yylval, &yylloc, scanner);

    if (token == SQL_IDENTIFIER || token == SQL_STRING) {
      free(yylval.sval);
    }
  }

  msql__delete_buffer(state, scanner);
  msql_lex_destroy(scanner);
  return true;
}

}  // namespace msql
