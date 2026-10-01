# Temporary compatibility adapter for the RoModularBuild migration.
# DspCore keeps its public cache variables while the shared implementation uses
# project-independent ROMODULAR_* names.

function(dspcore_forward_toolchain_cache
    DSPCORE_NAME
    ROMODULAR_NAME
    CACHE_TYPE
    DEFAULT_VALUE
    DESCRIPTION
)
    if(NOT DEFINED ${DSPCORE_NAME})
        if(DEFINED ${ROMODULAR_NAME})
            set(${DSPCORE_NAME} "${${ROMODULAR_NAME}}" CACHE ${CACHE_TYPE}
                "${DESCRIPTION}")
        else()
            set(${DSPCORE_NAME} "${DEFAULT_VALUE}" CACHE ${CACHE_TYPE}
                "${DESCRIPTION}")
        endif()
    endif()

    if(DEFINED ${ROMODULAR_NAME} AND
       NOT "${${ROMODULAR_NAME}}" STREQUAL "${${DSPCORE_NAME}}")
        message(WARNING
            "Both ${DSPCORE_NAME} and ${ROMODULAR_NAME} are set; "
            "DspCore compatibility gives ${DSPCORE_NAME} precedence"
        )
    endif()

    set(${ROMODULAR_NAME} "${${DSPCORE_NAME}}" CACHE ${CACHE_TYPE}
        "${DESCRIPTION}" FORCE)
endfunction()
