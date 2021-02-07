add_library(asio INTERFACE)

target_include_directories(asio INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_compile_definitions(asio INTERFACE ASIO_STANDALONE)
target_compile_features(asio INTERFACE cxx_std_11)
