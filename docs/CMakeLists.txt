option(BUILD_DOC "Build documentation" FALSE)

if(BUILD_DOC)
    find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
    if (DOXYGEN_FOUND)
        SET(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/docs/Doxyfile.in)
        SET(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

        add_custom_target( doc_doxygen ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "generating CubiCAD documentation via doxygen"
                VERBATIM)
    else(DOXYGEN_FOUND)
        message("Doxygen is required for building docs")
    endif(DOXYGEN_FOUND)
endif(BUILD_DOC)
