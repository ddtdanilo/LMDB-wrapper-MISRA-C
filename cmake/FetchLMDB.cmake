include(FetchContent)

FetchContent_Declare(
    lmdb
    GIT_REPOSITORY https://github.com/LMDB/lmdb.git
    GIT_TAG        LMDB_0.9.33
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(lmdb)

# LMDB does not provide a CMake build, so we create a library target manually
set(LMDB_SOURCE_DIR "${lmdb_SOURCE_DIR}/libraries/liblmdb")

add_library(lmdb_lib STATIC
    "${LMDB_SOURCE_DIR}/mdb.c"
    "${LMDB_SOURCE_DIR}/midl.c"
)

target_include_directories(lmdb_lib PUBLIC "${LMDB_SOURCE_DIR}")

# Suppress warnings in third-party code
target_compile_options(lmdb_lib PRIVATE -w)
