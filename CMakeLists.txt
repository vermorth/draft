cmake_minimum_required(VERSION 3.11)

project(draft LANGUAGES CXX C VERSION 0.1.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(3rdparty)
add_subdirectory(infra/http)
add_subdirectory(infra/ini_parser)
add_subdirectory(apps/coro_client)
add_subdirectory(apps/uring_tools)


