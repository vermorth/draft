
#include <stdlib.h>
#include <string>

// include the sql parser
#include "sql_parser.h"

// contains printing utilities
#include "util/sql_helper.h"

int main(int argc, char* argv[]) {
  if (argc <= 1) {
    fprintf(stderr, "Usage: ./example \"SELECT * FROM test;\"\n");
    return -1;
  }
  std::string query = argv[1];

  // parse a given query
  msql::SQLParserResult result;
  msql::SQLParser::Parse(query, &result);

  // check whether the parsing was successful
  if (result.is_valid()) {
    printf("Parsed successfully!\n");
    printf("Number of statements: %lu\n", result.statements_size());

    for (auto i = 0u; i < result.statements_size(); ++i) {
      // Print a statement summary.
      msql::PrintStatementInfo(result.statement(i));
    }
    return 0;
  } else {
    fprintf(stderr, "Given string is not a valid SQL query.\n");
    fprintf(stderr, "%s (L%d:%d)\n", result.error_msg(), result.error_line(),
            result.error_column());
    return -1;
  }
}
