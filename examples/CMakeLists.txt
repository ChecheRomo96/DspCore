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

if(DSPCORE_CORE)
    add_subdirectory(Core)
endif()

set(DSPCORE_EXAMPLES_LAUNCHER_DIRECTORY
    "${CMAKE_CURRENT_BINARY_DIR}/launchers")

set(DSPCORE_EXAMPLES_PLOT_PYTHON
    "${DSPCORE_ROOT_DIRECTORY}/examples/Tools/plot_signal.py")
set(DSPCORE_EXAMPLES_PLOT_MATLAB
    "${DSPCORE_ROOT_DIRECTORY}/examples/Tools/plot_signal.m")
set(DSPCORE_EXAMPLES_RENDER_SHELL
    "${DSPCORE_ROOT_DIRECTORY}/examples/Tools/render_signal.sh")
set(DSPCORE_EXAMPLES_RENDER_POWERSHELL
    "${DSPCORE_ROOT_DIRECTORY}/examples/Tools/render_signal.ps1")
add_custom_target(DspCoreExamplePlotters ALL
    COMMAND "${CMAKE_COMMAND}" -E make_directory
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DSPCORE_EXAMPLES_PLOT_PYTHON}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/plot_signal.py"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DSPCORE_EXAMPLES_PLOT_MATLAB}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/plot_signal.m"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DSPCORE_EXAMPLES_RENDER_SHELL}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/render_signal.sh"
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
        "${DSPCORE_EXAMPLES_RENDER_POWERSHELL}"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/render_signal.ps1")
add_dependencies(DspCoreExamples DspCoreExamplePlotters)
install(PROGRAMS "${DSPCORE_EXAMPLES_PLOT_PYTHON}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
    CONFIGURATIONS Release COMPONENT Examples)
install(FILES "${DSPCORE_EXAMPLES_PLOT_MATLAB}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
    CONFIGURATIONS Release COMPONENT Examples)
install(PROGRAMS "${DSPCORE_EXAMPLES_RENDER_SHELL}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
    CONFIGURATIONS Release COMPONENT Examples)
install(FILES "${DSPCORE_EXAMPLES_RENDER_POWERSHELL}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/examples"
    CONFIGURATIONS Release COMPONENT Examples)

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

if(DSPCORE_LUT)
    add_subdirectory(LUT)
endif()

if(DSPCORE_INTERPOLATION)
    add_subdirectory(Interpolation)
endif()

if(DSPCORE_GENERATORS)
    add_subdirectory(Generators)
endif()


if(DSPCORE_FILTERS)
    add_subdirectory(Filters)
endif()
if(DSPCORE_ANALYSIS)
    add_subdirectory(Analysis)
endif()
