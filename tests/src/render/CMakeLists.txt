
# Since renderer.test.cpp requires a window, skip for CI
if(NOT BUILD_FOR_CI)
    airship_test(renderer renderer.test.cpp DEPENDS AirshipRenderer)
endif()

airship_test(color color.test.cpp DEPENDS AirshipRenderer)
