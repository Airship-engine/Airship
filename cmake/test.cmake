
function(airship_test TARGET_NAME SOURCE_LIST)
    # Optional argument DISABLE_ENVIRONMENT disables the airship testbed for sensitive tests that knowingly go into bad states.
    # Useful for testing logging validity, or recovery from normally fatal situations.
    set(options DISABLE_ENVIRONMENT)
    cmake_parse_arguments(airship_test "${options}" "" "" ${ARGN})

    if(BUILD_TESTS)
        add_executable(${TARGET_NAME} "${SOURCE_LIST}")

        target_link_libraries(${TARGET_NAME}
            PRIVATE
                AirshipRenderer
                AirshipTest
                core
                gtest_main
        )
        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/test/include
        )
        if(airship_test_DISABLE_ENVIRONMENT)
            target_compile_definitions(${TARGET_NAME} PUBLIC DISABLE_TEST_ENVIRONMENT=1)
        endif()
        gtest_discover_tests(${TARGET_NAME})
	endif()
endfunction()

#   Configure unit tests    #
function(add_airship_test SOURCE)
    if(BUILD_TESTS)
        get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
        add_executable(${TEST_NAME} 
            ${SOURCE} 
            ${PROJECT_SOURCE_DIR}/test/include/test/common.h
        )
        # TODO: link to the required libraries for each test,
        # instead of to all of them.
        target_link_libraries(${TEST_NAME}
            PRIVATE
                AirshipRenderer
                AirshipTest
                core
                gtest_main
        )
        target_include_directories(${TEST_NAME}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/test/include
        )
        gtest_discover_tests(${TEST_NAME})
    endif()
endfunction()

if(BUILD_TESTS)
    #   Register unit tests    #
    include(GoogleTest)
endif()
