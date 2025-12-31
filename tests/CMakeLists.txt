#   Register unit tests     #
include(GoogleTest)

add_custom_target(AirshipTests)
set_target_properties(AirshipTests PROPERTIES FOLDER "Tests")

include(FetchContent)
FetchContent_Declare(
  googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723 # Release 1.17.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

foreach(tgt IN ITEMS gtest gtest_main gmock gmock_main)
  if(TARGET ${tgt})
    target_compile_options(${tgt} PRIVATE
      $<$<CXX_COMPILER_ID:Clang>:-w>
    )
    set_target_properties(${tgt} PROPERTIES FOLDER "Tests/Google")
  endif()
endforeach()

# Include airship_test function
include (tests.cmake)

add_subdirectory(src/core)
add_subdirectory(src/render)

