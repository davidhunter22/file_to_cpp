source_group( " "  REGULAR_EXPRESSION .*)

find_package(CLI11 CONFIG REQUIRED)

add_executable(file_to_cpp FileToCpp.cpp ${CMAKE_SOURCE_DIR}/README.md)

target_link_libraries(file_to_cpp PRIVATE CLI11::CLI11)

if (MSVC)
    set_target_properties(file_to_cpp PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/VS.props")
endif()

if(MSVC)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/24922
    set_target_properties(file_to_cpp PROPERTIES VS_GLOBAL_BuildStlModules "true")
endif()

