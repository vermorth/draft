Using the Library
=======================

To include it in your own code you only need to include one header file: sql_parser.h. The entire framework is wrapped in the namespace msql. To parse a SQL string you have to call the static method `bool msql::SQLParser::Parse(const std::string& sql, SQLParserResult* result)`.

The `parse` method will return an object of type `SQLParserResult`. When the query was valid SQL the result will contain a list of `SQLStatement` objects that represent the statements in your query. 

This is a list of the currently available statement types, each being a subclass of `SQLStatement`:

```
CreateStatement
DeleteStatement
DropStatement
ExecuteStatement
ImportStatement
PrepareStatement
SelectStatement
UpdateStatement
```

To find out what type of statement a certain `SQLStatement` is, you can check the `stmt->type()`, which will return an enum value. This `enum StatementType` is defined in `sql_statement.h`. There you can see all the available values. Some of these do not match to statement classes though, because they are not implemented yet.

Probably the best way to get familiar with the properties is to look at the class definitions itself in the repository here. The statement definitions are simply structs holding the data from the query. You could also take a look at the utility code in `sql_helper.cpp` which contains code that prints information about statements to the console.

## Example Code
```C++
// include the sql parser
#include "sql_parser.h"

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

    // process the statements...
  } else {
    fprintf(stderr, "Given string is not a valid SQL query.\n");
    fprintf(stderr, "%s (L%d:%d)\n", result.error_msg(), result.error_line(),
            result.error_column());
    return -1;
  }
  return 0;
}
```

