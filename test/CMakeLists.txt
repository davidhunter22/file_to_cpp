source_group( " "  REGULAR_EXPRESSION .*)

set(INPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/HelloWorld.txt")
set(OUTPUT_CPP_FILE "HelloWorld.cpp")
set(OUTPUT_HEADER_FILE "HelloWorld.h")

# Note that the file_to_cpp executable is found automatically by add_custom_command as it's a known add_executable target
add_custom_command(
    OUTPUT HelloWorld.cpp
    MAIN_DEPENDENCY ${INPUT_FILE}
    COMMAND file_to_cpp -i ${INPUT_FILE} --output_header
    BYPRODUCTS ${OUTPUT_HEADER_FILE}
    VERBATIM
    COMMENT "Generating embeded file for input ${INPUT_FILE}")

add_executable(hello_world
   Test.cpp
   HelloWorld.txt)

target_sources(hello_world PRIVATE ${OUTPUT_CPP_FILE})
target_sources(hello_world PRIVATE ${OUTPUT_HEADER_FILE})

target_include_directories(hello_world PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

# This stuff is need to get "import std;" to work
if (MSVC)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/24922
    set_target_properties(hello_world PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/VS.props")
    set_target_properties(hello_world PROPERTIES VS_GLOBAL_BuildStlModules "true")
endif()

