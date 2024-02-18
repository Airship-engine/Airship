#   Configure unit tests    #
function(add_airship_test name)
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}
        PRIVATE
            core
            gtest_main
    )
endfunction()

add_airship_test(testapp)
add_airship_test(logging)

#   Register unit tests    #
include(GoogleTest)
gtest_discover_tests(testapp)
