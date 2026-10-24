set(DSPCORE_MODULE_HEADERS "")

if(DSPCORE_CORE)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Core.h")
endif()

if(DSPCORE_PROCESSING)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Processing.h")
endif()

if(DSPCORE_FILTERS)
    list(APPEND DSPCORE_MODULE_HEADERS "${CMAKE_CURRENT_LIST_DIR}/Filters.h")
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

if(DSPCORE_PROCESSING)
    add_subdirectory(Processing)
endif()

if(DSPCORE_FILTERS)
    add_subdirectory(Filters)
endif()

