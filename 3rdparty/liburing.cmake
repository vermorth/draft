include(ExternalProject)
file(GLOB SRC_FILES_LIBURING "liburing/src/*.c")


file(MAKE_DIRECTORY ${LIBURING_CONFIGURE_INCLUDE_PATH}/liburing)
file(TOUCH ${LIBURING_CONFIGURE_INCLUDE_PATH}/liburing/config-host.h)

ExternalProject_Add(
        liburing-install
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/liburing
        PREFIX ${CMAKE_BINARY_DIR}/_deps
        CONFIGURE_COMMAND chmod +x ${CMAKE_CURRENT_SOURCE_DIR}/liburing/configure && ${CMAKE_CURRENT_SOURCE_DIR}/liburing/configure --prefix=${CMAKE_BINARY_DIR}/_deps/liburing/install
        INSTALL_COMMAND cmake -E echo "Skipping install step."
        BUILD_COMMAND cmake -E echo "Skipping build step.")

add_library(
        uring
        ${SRC_FILES_LIBURING})

add_dependencies(
        uring
        liburing-install)

target_compile_options(
        uring
        PRIVATE
        -include ${LIBURING_CONFIGURE_INCLUDE_PATH}/liburing/config-host.h -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare)

target_include_directories(
        uring
        PUBLIC
        liburing/src/include liburing/src/include/liburing ${LIBURING_CONFIGURE_INCLUDE_PATH} ${LIBURING_BUILD_PATH})

list(APPEND DEPS_LIST_LIBRARIES "uring")

