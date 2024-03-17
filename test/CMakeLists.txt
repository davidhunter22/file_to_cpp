# This is an example of a CMake project that uses the executable.
# It does so directly not using any pre defined CMake function or scripts
# This is totaly self contained, everything you need is right here

# Stop the generated Visual Studio project creating annoying sub folders like, "Source"
source_group( " "  REGULAR_EXPRESSION .*)

set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld.txt")
set(OUTPUT_CPP_FILE    "HelloWorld.cpp")
set(OUTPUT_HEADER_FILE "HelloWorld.h")

add_executable(file_to_cpp_test
   Test.cpp
   HelloWorld.txt)

set(CMD file_to_cpp -i ${INPUT_FILE} --output_header)

set(CMD ${CMD} --namespace Foo::Bar)

set(CMD ${CMD} -f get_hello_world_data)

message(STATUS "CMD=${CMD}")

# Note that the 'file_to_cpp' executable is found automatically by add_custom_command as it's a known target created by an add_executable command
add_custom_command(
    OUTPUT ${OUTPUT_CPP_FILE} ${OUTPUT_HEADER_FILE}
    MAIN_DEPENDENCY ${INPUT_FILE}
    COMMAND ${CMD}
    BYPRODUCTS ${OUTPUT_HEADER_FILE}
    VERBATIM
    COMMENT "Generating embeded file for input ${INPUT_FILE}")

# Add both generated files to the target, add_custom_command does not do this
# add_custom_command does add the GENERATED source file proerty to both these files so there is no need to do that
target_sources(file_to_cpp_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_CPP_FILE})
target_sources(file_to_cpp_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_HEADER_FILE})

# To #include the header file, which is in the build tree, we need to add that directory to the build path
target_include_directories(file_to_cpp_test PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# This stuff is need to get "import std;" to work
if (MSVC)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/24922
    set_target_properties(file_to_cpp_test PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/VS.props")
    set_target_properties(file_to_cpp_test PROPERTIES VS_GLOBAL_BuildStlModules "true")
endif()

