
add_library(infra_ini_parser STATIC ini_parser.cc)

target_include_directories(infra_ini_parser PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_features(infra_ini_parser PRIVATE cxx_std_14)

add_executable(infra_ini_parser_test ini_parser_test.cc)

target_include_directories(infra_ini_parser_test PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(infra_ini_parser_test PRIVATE GTest::gtest infra_ini_parser)
target_compile_features(infra_ini_parser_test PRIVATE cxx_std_14)
