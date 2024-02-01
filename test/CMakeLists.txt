include(FetchContent)

# Get googletest and configure it
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#   Configure unit tests    #
add_executable(testapp testapp.cpp)
target_link_libraries(testapp
    PRIVATE
        core
        gtest_main
)

#   Register unit tests    #
include(GoogleTest)
gtest_discover_tests(testapp)
