cmake_minimum_required(VERSION 3.14)
project(lmdb_wrapper C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Strict compiler warnings (MISRA C aligned)
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wshadow
    -Wconversion
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wno-unused-parameter
)

# Fetch LMDB
include(cmake/FetchLMDB.cmake)

# Fetch CMocka
include(cmake/FetchCMocka.cmake)

# Wrapper library
add_library(lmdb_wrapper STATIC
    src/lmdb_wrapper.c
)

target_include_directories(lmdb_wrapper
    PUBLIC include
    PRIVATE "${LMDB_SOURCE_DIR}"
)

target_link_libraries(lmdb_wrapper PRIVATE lmdb_lib)

# Tests
enable_testing()

add_executable(test_lmdb_wrapper tests/test_lmdb_wrapper.c)

target_link_libraries(test_lmdb_wrapper PRIVATE lmdb_wrapper lmdb_lib ${CMOCKA_LIB})

target_include_directories(test_lmdb_wrapper
    PRIVATE include
    PRIVATE "${LMDB_SOURCE_DIR}"
    PRIVATE "${CMOCKA_INCLUDE_DIR}"
)

add_test(
    NAME lmdb_wrapper_tests
    COMMAND test_lmdb_wrapper
)

# Example program
add_executable(example_basic_usage examples/basic_usage.c)

target_link_libraries(example_basic_usage PRIVATE lmdb_wrapper lmdb_lib)

target_include_directories(example_basic_usage
    PRIVATE include
    PRIVATE "${LMDB_SOURCE_DIR}"
)
