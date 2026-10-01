find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)

    set(DOXYGEN_IN  ${DSPCORE_ROOT_DIRECTORY}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/docs/Doxyfile)
    set(DOXYGEN_HTML_FOOTER
        ${DSPCORE_ROOT_DIRECTORY}/docs/assets/DspCoreFooter.html)
    set(DOXYGEN_HTML_EXTRA_FILES
        ${DSPCORE_ROOT_DIRECTORY}/docs/assets/DspCoreDocs.js)

    add_subdirectory(${DSPCORE_ROOT_DIRECTORY}/docs)

    get_target_property(DSPCORE_DOXYGEN_PREDEFS DspCore DSPCORE_DOXYGEN_PREDEFS)
    get_target_property(DSPCORE_DOXYGEN_INPUTS DspCore DSPCORE_DOXYGEN_INPUTS)

    if(NOT DSPCORE_DOXYGEN_PREDEFS)
        set(DSPCORE_DOXYGEN_PREDEFS "")
    endif()

    if(NOT DSPCORE_DOXYGEN_INPUTS)
        set(DSPCORE_DOXYGEN_INPUTS "")
    endif()

    # Doxygen does not run a C++ compiler, so it cannot infer the language
    # feature-test value selected by the DspCore target. Keep C++17 declarations
    # and documentation-only preprocessor paths visible.
    list(APPEND DSPCORE_DOXYGEN_PREDEFS
        DOXYGEN=1
        DSPCORE_CPLUSPLUS=201703L
    )

    string(REPLACE ";" " " DOXYGEN_PREDEFINED "${DSPCORE_DOXYGEN_PREDEFS}")
    string(REPLACE ";" " " DOXYGEN_INPUT "${DSPCORE_DOXYGEN_INPUTS}")

    message(STATUS "Doxygen Predefined:")
    foreach(item IN LISTS DSPCORE_DOXYGEN_PREDEFS)
        message(STATUS "  ${item}")
    endforeach()

    message(STATUS "Doxygen Inputs:")
    foreach(item IN LISTS DSPCORE_DOXYGEN_INPUTS)
        message(STATUS "  ${item}")
    endforeach()

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    message(STATUS "Doxygen configuration file created at ${DOXYGEN_OUT}")

    add_custom_target(DspCoreDocs ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${DSPCORE_ROOT_DIRECTORY}
        COMMENT "Generating DspCore API documentation with Doxygen"
        VERBATIM
    )

    add_custom_target(docs DEPENDS DspCoreDocs)

else()
    message(WARNING "Doxygen is required to build the documentation.")
endif()
