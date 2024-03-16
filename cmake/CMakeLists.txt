# This project just wraps up some of the add_custom command into a CMake function
# This simplifies the calling site somewhat and stops you forgetting to do things like
# adding the output files to the target, or extending the include path
include(FileToCpp.cmake)

source_group( " "  REGULAR_EXPRESSION .*)

add_executable(file_to_cpp_cmake
   Test.cpp
   HelloWorld.txt
   FileToCpp.cmake)

call_file_to_cpp(VERBOSE TARGET file_to_cpp_cmake FILE HelloWorld.txt)

# # This stuff is need to get "import std;" to work
# if (MSVC)
#     # https://gitlab.kitware.com/cmake/cmake/-/issues/24922
#     set_target_properties(hello_world PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/VS.props")
#     set_target_properties(hello_world PROPERTIES VS_GLOBAL_BuildStlModules "true")
# endif()

