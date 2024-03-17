source_group( " "  REGULAR_EXPRESSION .*)

add_library(file_to_cpp_lib STATIC 
    FileToCpp.cpp 
    FileToCpp.h
    ReadFile.cpp 
    ReadFile.h)

if (MSVC)
    # https://gitlab.kitware.com/cmake/cmake/-/issues/24922
    set_target_properties(file_to_cpp_lib PROPERTIES VS_USER_PROPS "${CMAKE_SOURCE_DIR}/VS.props")
    set_target_properties(file_to_cpp_lib PROPERTIES VS_GLOBAL_BuildStlModules "true")	
endif()


