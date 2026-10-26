set(DSPCORE_PROCESSING_HEADERS
    "${CMAKE_CURRENT_LIST_DIR}/ProcessorBase.h"
    "${CMAKE_CURRENT_LIST_DIR}/SignalBlock.h"
    "${CMAKE_CURRENT_LIST_DIR}/SignalManager.h")

target_sources(DspCore PRIVATE ${DSPCORE_PROCESSING_HEADERS})
dspcore_stage_headers(${DSPCORE_PROCESSING_HEADERS})
dspcore_add_dox(${DSPCORE_PROCESSING_HEADERS}
    "${CMAKE_CURRENT_LIST_DIR}/Processing.dox")

install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/ProcessorBase.h"
    "${CMAKE_CURRENT_LIST_DIR}/SignalBlock.h"
    "${CMAKE_CURRENT_LIST_DIR}/SignalManager.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/DspCore/Processing"
    CONFIGURATIONS Release)
