file(GLOB_RECURSE CoroClientSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_executable(coro_client ${CoroClientSources})

target_include_directories(coro_client PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(coro_client PRIVATE asio infra_http pthread)
target_compile_features(coro_client PRIVATE cxx_std_14)
