include(FetchContent)

FetchContent_Declare(
    cmocka
    GIT_REPOSITORY https://gitlab.com/cmocka/cmocka.git
    GIT_TAG        cmocka-1.1.7
    GIT_SHALLOW    TRUE
)

set(WITH_STATIC_LIB ON CACHE BOOL "Build cmocka static library" FORCE)
set(WITH_CMOCKERY_SUPPORT OFF CACHE BOOL "" FORCE)
set(WITH_EXAMPLES OFF CACHE BOOL "" FORCE)
set(UNIT_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(cmocka)

# Determine which cmocka target is available and suppress warnings
if(TARGET cmocka-static)
    set(CMOCKA_LIB cmocka-static CACHE STRING "" FORCE)
    target_compile_options(cmocka-static PRIVATE -w)
elseif(TARGET cmocka)
    set(CMOCKA_LIB cmocka CACHE STRING "" FORCE)
    target_compile_options(cmocka PRIVATE -w)
else()
    message(FATAL_ERROR "CMocka library target not found")
endif()

set(CMOCKA_INCLUDE_DIR "${cmocka_SOURCE_DIR}/include" CACHE STRING "" FORCE)
