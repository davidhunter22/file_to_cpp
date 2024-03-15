cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 26)

project(file_to_cpp)

set_property(GLOBAL PROPERTY USE_FOLDERS "On")

add_subdirectory(src)
add_subdirectory(test)

