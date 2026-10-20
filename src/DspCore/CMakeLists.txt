set(DSPCORE_MODULE_HEADERS "")

if(DSPCORE_CORE)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Core.h")
endif()

if(DSPCORE_LUT)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/LUT.h")
endif()

if(DSPCORE_INTERPOLATION)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Interpolation.h")
endif()

if(DSPCORE_GENERATORS)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Generators.h")
endif()


if(DSPCORE_FILTERS)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Filters.h")
endif()

if(DSPCORE_ANALYSIS)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Analysis.h")
endif()

if(DSPCORE_PROCESSING)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Processing.h")
endif()

target_sources(DspCore PRIVATE ${DSPCORE_MODULE_HEADERS})
dspcore_stage_headers(${DSPCORE_MODULE_HEADERS})
dspcore_add_dox(${DSPCORE_MODULE_HEADERS})

install(FILES ${DSPCORE_MODULE_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/DspCore"
    CONFIGURATIONS Release)

if(DSPCORE_CORE)
    add_subdirectory(Core)
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

if(DSPCORE_PROCESSING)
    add_subdirectory(Processing)
endif()


if(DSPCORE_FILTERS OR DSPCORE_ANALYSIS)
    set(DSPCORE_SIGNAL_DETAIL "${CMAKE_CURRENT_LIST_DIR}/Detail/SignalMath.h")
    target_sources(DspCore PRIVATE ${DSPCORE_SIGNAL_DETAIL})
    dspcore_stage_headers(${DSPCORE_SIGNAL_DETAIL})
    install(FILES ${DSPCORE_SIGNAL_DETAIL}
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/DspCore/Detail" CONFIGURATIONS Release)
endif()
