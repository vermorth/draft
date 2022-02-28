file(GLOB_RECURSE UringToolsSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

add_executable(uring_tools ${UringToolsSources})

target_include_directories(uring_tools PRIVATE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(uring_tools PRIVATE uring)
target_compile_features(uring_tools PRIVATE cxx_std_14)
