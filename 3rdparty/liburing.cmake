include(ExternalProject)
file(GLOB SRC_FILES_LIBURING "liburing/src/*.c")


set(LIBURING_CONFIGURE_PREIFX_DIR "${CMAKE_BINARY_DIR}/liburing-configure-prefix")
file(MAKE_DIRECTORY ${LIBURING_CONFIGURE_PREIFX_DIR})

ExternalProject_Add(
    liburing-configure
    BUILD_IN_SOURCE 1
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liburing
    PREFIX ${LIBURING_CONFIGURE_PREIFX_DIR}
    CONFIGURE_COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/liburing/configure && ${CMAKE_CURRENT_SOURCE_DIR}/liburing/configure --prefix=${LIBURING_CONFIGURE_PREIFX_DIR}
    INSTALL_COMMAND cmake -E echo "Skipping install step."
    BUILD_COMMAND cmake -E echo "Skipping build step.")

add_library(
    uring
    ${SRC_FILES_LIBURING})

add_dependencies(
    uring
    liburing-configure)

target_compile_options(
    uring
    PRIVATE
    -D_GNU_SOURCE -include ${CMAKE_CURRENT_SOURCE_DIR}/liburing/config-host.h -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare)

target_include_directories(
    uring
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/liburing
    ${CMAKE_CURRENT_SOURCE_DIR}/liburing/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/liburing/src/include/liburing)

