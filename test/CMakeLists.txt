
#   Configure unit tests    #
add_executable(testapp testapp.cpp)
target_link_libraries(testapp PUBLIC core)
target_include_directories(testapp PUBLIC ${PROJECT_SOURCE_DIR}/src)

target_compile_features(testapp PUBLIC cxx_std_20)

#   Register unit tests    #
add_test(Basic.CreateWindow_is_not_null testapp)
