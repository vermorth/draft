#ifndef __HELPER_H__
#define __HELPER_H__

#define TEST_PARSE_SQL_QUERY(query, result, numStatements) \
  msql::SQLParserResult result;                            \
  msql::SQLParser::Parse(query, &result);                  \
  ASSERT(result.is_valid());                                \
  ASSERT_EQ(result.statements_size(), numStatements);

#define TEST_PARSE_SINGLE_SQL(query, stmtType, stmtClass, result, outputVar) \
  TEST_PARSE_SQL_QUERY(query, result, 1);                                    \
  ASSERT_EQ(result.statement(0)->type(), stmtType);                       \
  const stmtClass* outputVar = (const stmtClass*)result.statement(0);

#define TEST_CAST_STMT(result, stmt_index, stmtType, stmtClass, outputVar) \
  ASSERT_EQ(result.statement(stmt_index)->type(), stmtType);            \
  const stmtClass* outputVar =                                             \
      (const stmtClass*)result.statement(stmt_index);

#endif
