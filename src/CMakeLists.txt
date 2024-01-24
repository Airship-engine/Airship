# Arbitrary somewhat-recent cmake version
cmake_minimum_required(VERSION 3.20)

project(main)

add_executable(main main.cpp)

# To be added once we need to link libraries
# target_link_libraries(main PRIVATE hello)

# Tell C++ compiler to use C++20 features
target_compile_features(main PUBLIC cxx_std_20)