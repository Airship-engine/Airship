
#   Configure unit tests    #
function(add_airship_test SOURCE)
    if(BUILD_TESTS)
        get_filename_component(TEST_NAME ${SOURCE} NAME_WLE)
        add_executable(${TEST_NAME} 
            ${SOURCE} 
            ${PROJECT_SOURCE_DIR}/test/include/test/common.h
        )
        target_link_libraries(${TEST_NAME}
            PRIVATE
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
