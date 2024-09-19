include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
option(INSTALL_GTEST "Install GTest" OFF)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

macro(AddTests target)
    message(STATUS "Adding tests to ${target}")
    target_link_libraries(${target} PRIVATE gtest_main)
    gtest_discover_tests(${target})
endmacro()
