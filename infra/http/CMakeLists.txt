
file(GLOB_RECURSE MySources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_library(infra_http STATIC ${MySources})

target_include_directories(infra_http PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(infra_http PRIVATE asio)
target_compile_features(infra_http PRIVATE cxx_std_14)
