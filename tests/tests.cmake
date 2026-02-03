function(airship_test TARGET_NAME SOURCE_LIST)
    # Optional argument DISABLE_ENVIRONMENT disables the airship testbed for sensitive tests that knowingly go into bad states.
    # Useful for testing logging validity, or recovery from normally fatal situations.
    set(options DISABLE_ENVIRONMENT)
    set(list_options DEPENDS)
    cmake_parse_arguments(airship_test "${options}" "" "${list_options}" ${ARGN})

    if(BUILD_TESTS)
        add_executable(${TARGET_NAME})
        enable_clang_tidy(${TARGET_NAME})
        set_target_properties(${TARGET_NAME} PROPERTIES FOLDER "Tests")

        target_link_libraries(${TARGET_NAME}
            PRIVATE
                AirshipCore
                AirshipCommonFlags
                GTest::gtest_main
                ${airship_test_DEPENDS}
        )
        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${PROJECT_SOURCE_DIR}/tests/include
        )
        if(airship_test_DISABLE_ENVIRONMENT)
            target_compile_definitions(${TARGET_NAME} PUBLIC DISABLE_TEST_ENVIRONMENT=1)
        endif()

        target_sources(${TARGET_NAME} 
            PRIVATE 
                "${SOURCE_LIST}"
                ${PROJECT_SOURCE_DIR}/tests/src/test/common.cpp 
        )
        target_sources(${TARGET_NAME} PUBLIC
            ${PROJECT_SOURCE_DIR}/tests/include/test/common.h
        )
        source_group("" FILES ${SOURCE_LIST})
        source_group("Common" 
            FILES
                ${PROJECT_SOURCE_DIR}/tests/src/test/common.cpp 
                ${PROJECT_SOURCE_DIR}/tests/include/test/common.h
        )

        gtest_discover_tests(${TARGET_NAME})
        add_dependencies(AirshipTests ${TARGET_NAME})
    endif()
endfunction()

