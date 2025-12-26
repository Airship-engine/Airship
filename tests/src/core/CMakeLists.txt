
set(CORE_TEST_SOURCES
    convar.test.cpp
    event.test.cpp
)

if(NOT BUILD_FOR_CI)
    list(APPEND CORE_TEST_SOURCES application.test.cpp)
endif()

airship_test(core_test "${CORE_TEST_SOURCES}")
airship_test(logging logging.test.cpp DISABLE_ENVIRONMENT)
