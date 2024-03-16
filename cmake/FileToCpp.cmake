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

    get_filename_component(FILE_WITHOUT_EXT ${ARGS_FILE} NAME_WE)
    set(OUTPUT_CPP_FILE    ${FILE_WITHOUT_EXT}.cpp)
    set(OUTPUT_HEADER_FILE ${FILE_WITHOUT_EXT}.h)

    if(NOT ${ARGS_NAMESPACE} STREQUAL "")
        set(ARGS_NAMESPACE --namespace ${ARGS_NAMESPACE})
    endif()

    # We assume that you are using the CMake generated declspec stuff this uses and upper case target name with "_EXPORT" as the declspec macro
    string(TOUPPER ${ARGS_TARGET} TARGET_NAME_UPPER)
    set(DECLSPEC_HEADER --declspec_header ${ARGS_TARGET}_export.h)
    set(DECLSPEC_MACRO  --declspec_macro ${TARGET_NAME_UPPER}_EXPORT)

    if(ARGS_VERBOSE)
        message(STATUS "Running command `file_to_cpp -i ${ARGS_FILE} ${ARGS_NAMESPACE} --output_header  ${DECLSPEC_HEADER} ${DECLSPEC_MACRO}`")
    endif()

    # Note that the 'file_to_cpp' executable is found automatically by add_custom_command as it's a known target created by an add_executable command
    add_custom_command(
        OUTPUT ${OUTPUT_CPP_FILE} ${OUTPUT_HEADER_FILE}
        MAIN_DEPENDENCY ${ARGS_FILE}
        COMMAND file_to_cpp -i ${ARGS_FILE} ${ARGS_NAMESPACE}  --output_header
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