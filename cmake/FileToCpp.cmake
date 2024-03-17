include_guard()

function(call_file_to_cpp)
    cmake_parse_arguments(
        ARGS
        "VERBOSE"
        "TARGET;FILE;NAMESPACE"
        ""
        "${ARGN}")

    if(ARGS_VERBOSE)
        message(STATUS "Running call_file_to_cpp command")
    endif()

    if("${ARGS_FILE}" STREQUAL "")
        message(FATAL "No input filename passed to call_file_to_cpp")
    endif()

    # We build up the command to run in the CMD variable
    set(CMD file_to_cpp)

    # Add the required input file
    set(CMD ${CMD} -i ${ARGS_FILE})

    if(NOT ${ARGS_NAMESPACE} STREQUAL "")
        set(CMD ${CMD} --namespace ${ARGS_NAMESPACE})
    endif()

    # We always output a header file
    set(CMD ${CMD} --output_header)

    get_filename_component(FILE_WITHOUT_EXT ${ARGS_FILE} NAME_WE)
    set(OUTPUT_CPP_FILE    ${FILE_WITHOUT_EXT}.cpp)
    set(OUTPUT_HEADER_FILE ${FILE_WITHOUT_EXT}.h)

    # We assume that you are using the CMake generated declspec stuff this uses and upper case target name with "_EXPORT" as the declspec macro
    string(TOUPPER ${ARGS_TARGET} TARGET_NAME_UPPER)
    # set(CMD ${CMD} --declspec_header ${ARGS_TARGET}_export.h)
    # set(CMD ${CMD} --declspec_macro  ${TARGET_NAME_UPPER}_EXPORT)

    if(ARGS_VERBOSE)
        message(STATUS "Running command ${CMD}")
    endif()

    # Note that the 'file_to_cpp' executable is found automatically by add_custom_command as it's a known target created by an add_executable command
    add_custom_command(
        OUTPUT ${OUTPUT_CPP_FILE} ${OUTPUT_HEADER_FILE}
        MAIN_DEPENDENCY ${ARGS_FILE}
        COMMAND ${CMD}
        BYPRODUCTS ${OUTPUT_HEADER_FILE}
        VERBATIM
        COMMENT "Generating embeded file for input ${ARGS_FILE}")

    # Add both generated files to the target, add_custom_command does not do this
    # add_custom_command does add the GENERATED source file proerty to both these files so there is no need to do that
    target_sources(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_CPP_FILE})
    target_sources(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUT_HEADER_FILE})

    # To #include the header file, which is in the build tree, we need to add that directory to the build path
    target_include_directories(${ARGS_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

endfunction()