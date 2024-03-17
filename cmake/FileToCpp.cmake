# This is an example of wrapping the 'file_to_cpp' executable in a CMake funtion
# Mostly this makes the calling site simpler for the user

include_guard()

if(TARGET file_to_cpp)
    set(FILE_TO_CPP_EXECUTABLE file_to_cpp)
else()
    if(WIN32)
        set(FILE_TO_CPP_EXECUTABLE ${CMAKE_SOURCE_DIR}/file_to_cpp/file_to_cpp.exe)
    else()
        set(FILE_TO_CPP_EXECUTABLE ${CMAKE_SOURCE_DIR}/file_to_cpp/file_to_cpp)
    endif()
endif()

function(call_file_to_cpp)
    cmake_parse_arguments(
        ARGS
        "VERBOSE;DECLSPEC"
        "TARGET;INPUT_FILE;OUTPUT_FILE_BASE;NAMESPACE;FUNCTION_NAME"
        ""
        "${ARGN}")

    if(DEFINED ARGS_UNPARSED_ARGUMENTS)
        message(FATAL "call_file_to_cpp has UNPARSED_ARGUMENTS: ${ARGS_UNPARSED_ARGUMENTS}")
    endif()

    if(ARGS_VERBOSE)
        message(STATUS "Running call_file_to_cpp command")
    endif()

    if("${ARGS_INPUT_FILE}" STREQUAL "")
        message(FATAL "No input filename passed to call_file_to_cpp")
    endif()

    # We build up the command to run in the CMD variable
    set(CMD ${FILE_TO_CPP_EXECUTABLE})

    # If the input file doesn't exist and the file path is not absolute try looking from the current source directory
    if(NOT EXISTS ${ARGS_INPUT_FILE} AND NOT IS_ABSOLUTE ${ARGS_INPUT_FILE})
         set(ARGS_INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARGS_INPUT_FILE})
    endif()

    # Add the required input file
    set(CMD ${CMD} -i ${ARGS_INPUT_FILE})

    if(DEFINED ARGS_NAMESPACE)
        set(CMD ${CMD} --namespace ${ARGS_NAMESPACE})
    endif()

    if(DEFINED ARGS_FUNCTION_NAME)
        set(CMD ${CMD} -f ${ARGS_FUNCTION_NAME})
    endif()

    # We always output a header file
    set(CMD ${CMD} --output_header)

    if(NOT DEFINED ARGS_OUTPUT_FILE_BASE)
         get_filename_component(ARGS_OUTPUT_FILE_BASE ${ARGS_INPUT_FILE} NAME_WE)
    endif()

    set(CMD ${CMD} -o ${ARGS_OUTPUT_FILE_BASE})

    # We assume that you are using the CMake generated declspec stuff this uses and upper case target name with "_EXPORT" as the declspec macro
    if(ARGS_DECLSPEC)
        string(TOUPPER ${ARGS_TARGET} TARGET_NAME_UPPER)
        set(CMD ${CMD} --declspec_header ${ARGS_TARGET}_export.h)
        set(CMD ${CMD} --declspec_macro  ${TARGET_NAME_UPPER}_EXPORT)
    endif()

    if(ARGS_VERBOSE)
        message(STATUS "Running command ${CMD}")
    endif()

    # Note that the 'file_to_cpp' executable is found automatically by add_custom_command as it's a known target created by an add_executable command
    add_custom_command(
        OUTPUT ${ARGS_OUTPUT_FILE_BASE}.cpp ${ARGS_OUTPUT_FILE_BASE}.h
        MAIN_DEPENDENCY ${ARGS_INPUT_FILE}
        COMMAND ${CMD}
        BYPRODUCTS ${OUTPUT_HEADER_FILE}
        VERBATIM
        COMMENT "Generating embeded file for input ${ARGS_INPUT_FILE}")

    # Add both generated files to the target, add_custom_command does not do this
    # add_custom_command does add the GENERATED source file property to both these files so there is no need to do that
    target_sources(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${ARGS_OUTPUT_FILE_BASE}.cpp)
    target_sources(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${ARGS_OUTPUT_FILE_BASE}.h)

    # To #include the header file, which is in the build tree, we need to add that directory to the build path
    target_include_directories(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

endfunction()