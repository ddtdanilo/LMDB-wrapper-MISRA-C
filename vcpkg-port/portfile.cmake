vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO ddtdanilo/LMDB-wrapper-MISRA-C
    REF "v${VERSION}"
    SHA512 0  # Update with actual SHA512 after release
    HEAD_REF main
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DLMDB_WRAPPER_BUILD_TESTS=OFF
        -DLMDB_WRAPPER_BUILD_EXAMPLES=OFF
)

vcpkg_cmake_build()
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME lmdb_wrapper CONFIG_PATH lib/cmake/lmdb_wrapper)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
