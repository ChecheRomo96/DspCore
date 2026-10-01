add_custom_target(DspCoreExamples)

function(dspcore_add_example TARGET_NAME)
    if(ARGC LESS 2)
        message(FATAL_ERROR
            "dspcore_add_example(${TARGET_NAME}) requires at least one source file")
    endif()

    add_executable(${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} PRIVATE DspCore::DspCore)
    set_target_properties(${TARGET_NAME} PROPERTIES DEBUG_POSTFIX "_d")
    add_dependencies(DspCoreExamples ${TARGET_NAME})

    install(TARGETS ${TARGET_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
        CONFIGURATIONS Release
        COMPONENT Examples)
endfunction()

set(DSPCORE_DESKTOP_EXAMPLES_SUPPORTED OFF)
if(APPLE OR WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(DSPCORE_DESKTOP_EXAMPLES_SUPPORTED ON)
endif()

add_subdirectory(DspCore)

set(DSPCORE_EXAMPLES_LAUNCHER_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/launchers")

if(APPLE)
    set(DSPCORE_EXAMPLES_LAUNCHER
        "${DSPCORE_EXAMPLES_LAUNCHER_DIRECTORY}/DspCoreExamples.command")
    configure_file(
        "${DSPCORE_ROOT_DIRECTORY}/examples/Launchers/macOS.command.in"
        "${DSPCORE_EXAMPLES_LAUNCHER}"
        @ONLY NEWLINE_STYLE UNIX)
    install(PROGRAMS "${DSPCORE_EXAMPLES_LAUNCHER}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
        CONFIGURATIONS Release COMPONENT Examples)
elseif(WIN32)
    set(DSPCORE_EXAMPLES_LAUNCHER
        "${DSPCORE_EXAMPLES_LAUNCHER_DIRECTORY}/DspCoreExamples.cmd")
    configure_file(
        "${DSPCORE_ROOT_DIRECTORY}/examples/Launchers/Windows.cmd.in"
        "${DSPCORE_EXAMPLES_LAUNCHER}"
        @ONLY NEWLINE_STYLE WIN32)
    install(PROGRAMS "${DSPCORE_EXAMPLES_LAUNCHER}"
        DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
        CONFIGURATIONS Release COMPONENT Examples)
endif()
