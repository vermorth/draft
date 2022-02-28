add_subdirectory(asio-1.18.1)
add_subdirectory(googletest-1.11.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
include("liburing")