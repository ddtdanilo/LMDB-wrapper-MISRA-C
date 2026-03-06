cmake_minimum_required(VERSION 3.14)
project(lmdb_wrapper
    VERSION 1.0.0
    LANGUAGES C
    DESCRIPTION "MISRA C compliant wrapper for LMDB"
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Options
option(LMDB_WRAPPER_BUILD_TESTS "Build tests" ON)
option(LMDB_WRAPPER_BUILD_EXAMPLES "Build examples" ON)

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

# --- LMDB dependency ---
# Try find_package first (vcpkg / system), then raw paths, then FetchContent
set(LMDB_WRAPPER_FETCHED_LMDB OFF)

find_path(LMDB_INCLUDE_DIR NAMES lmdb.h)
find_library(LMDB_LIBRARY NAMES lmdb)

if(LMDB_INCLUDE_DIR AND LMDB_LIBRARY)
    # Found via system or vcpkg
    set(LMDB_WRAPPER_LMDB_INCLUDE "${LMDB_INCLUDE_DIR}")
    set(LMDB_WRAPPER_LMDB_LIB "${LMDB_LIBRARY}")
else()
    # Standalone mode: fetch LMDB source
    set(LMDB_WRAPPER_FETCHED_LMDB ON)
    include(cmake/FetchLMDB.cmake)
    set(LMDB_WRAPPER_LMDB_INCLUDE "${LMDB_SOURCE_DIR}")
endif()

# --- Wrapper library ---
add_library(lmdb_wrapper STATIC
    src/lmdb_wrapper.c
)

target_include_directories(lmdb_wrapper
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        "${LMDB_WRAPPER_LMDB_INCLUDE}"
)

if(LMDB_WRAPPER_FETCHED_LMDB)
    target_link_libraries(lmdb_wrapper PRIVATE lmdb_lib)
else()
    target_link_libraries(lmdb_wrapper PRIVATE "${LMDB_WRAPPER_LMDB_LIB}")
endif()

set_target_properties(lmdb_wrapper PROPERTIES
    PUBLIC_HEADER include/lmdb_wrapper.h
    OUTPUT_NAME lmdb_wrapper
)

# --- Install rules ---
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

if(NOT LMDB_WRAPPER_FETCHED_LMDB)
    # Only export when using system/vcpkg lmdb (no internal targets to export)
    install(TARGETS lmdb_wrapper
        EXPORT lmdb_wrapper-targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(EXPORT lmdb_wrapper-targets
        FILE lmdb_wrapper-targets.cmake
        NAMESPACE lmdb_wrapper::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lmdb_wrapper
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/lmdb_wrapper-config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/lmdb_wrapper-config.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lmdb_wrapper
    )

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/lmdb_wrapper-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/lmdb_wrapper-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/lmdb_wrapper-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lmdb_wrapper
    )
else()
    # Standalone mode: basic install without cmake config export
    install(TARGETS lmdb_wrapper
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )
endif()

# --- Tests ---
if(LMDB_WRAPPER_BUILD_TESTS)
    include(cmake/FetchCMocka.cmake)
    enable_testing()

    add_executable(test_lmdb_wrapper tests/test_lmdb_wrapper.c)

    target_include_directories(test_lmdb_wrapper
        PRIVATE include
        PRIVATE "${LMDB_WRAPPER_LMDB_INCLUDE}"
        PRIVATE "${CMOCKA_INCLUDE_DIR}"
    )

    if(LMDB_WRAPPER_FETCHED_LMDB)
        target_link_libraries(test_lmdb_wrapper PRIVATE lmdb_wrapper lmdb_lib ${CMOCKA_LIB})
    else()
        target_link_libraries(test_lmdb_wrapper PRIVATE lmdb_wrapper "${LMDB_WRAPPER_LMDB_LIB}" ${CMOCKA_LIB})
    endif()

    add_test(
        NAME lmdb_wrapper_tests
        COMMAND test_lmdb_wrapper
    )
endif()

# --- Example ---
if(LMDB_WRAPPER_BUILD_EXAMPLES)
    add_executable(example_basic_usage examples/basic_usage.c)

    target_include_directories(example_basic_usage
        PRIVATE include
        PRIVATE "${LMDB_WRAPPER_LMDB_INCLUDE}"
    )

    if(LMDB_WRAPPER_FETCHED_LMDB)
        target_link_libraries(example_basic_usage PRIVATE lmdb_wrapper lmdb_lib)
    else()
        target_link_libraries(example_basic_usage PRIVATE lmdb_wrapper "${LMDB_WRAPPER_LMDB_LIB}")
    endif()
endif()
