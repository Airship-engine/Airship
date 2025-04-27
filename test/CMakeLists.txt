#   Register unit tests     #
include(GoogleTest)

add_custom_target(AirshipTests)

function(airship_test TARGET_NAME SOURCE_LIST)
    # Optional argument DISABLE_ENVIRONMENT disables the airship testbed for sensitive tests that knowingly go into bad states.
    # Useful for testing logging validity, or recovery from normally fatal situations.
    set(options DISABLE_ENVIRONMENT)
    set(list_options DEPENDS)
    cmake_parse_arguments(airship_test "${options}" "" "${list_options}" ${ARGN})

    if(BUILD_TESTS)
        add_executable(${TARGET_NAME} "${SOURCE_LIST}")

        target_link_libraries(${TARGET_NAME}
            PRIVATE
                AirshipTestUtils
                ${airship_test_DEPENDS}
        )
        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/test/include
        )
        if(airship_test_DISABLE_ENVIRONMENT)
            target_compile_definitions(${TARGET_NAME} PUBLIC DISABLE_TEST_ENVIRONMENT=1)
        endif()
        gtest_discover_tests(${TARGET_NAME})
        add_dependencies(AirshipTests ${TARGET_NAME})
    endif()
endfunction()

add_subdirectory(src/test)
