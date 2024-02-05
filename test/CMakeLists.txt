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
