# DspCore compatibility wrapper for RoModularBuild.
# Target profiles continue to set the established DSPCORE_* variables.

include("${CMAKE_CURRENT_LIST_DIR}/DspCoreRoModularCompatibility.cmake")

dspcore_forward_toolchain_cache(DSPCORE_AVR_TOOLCHAIN_ROOT ROMODULAR_AVR_TOOLCHAIN_ROOT
    PATH "" "Optional AVR-GCC installation root")
dspcore_forward_toolchain_cache(DSPCORE_AVR_TOOLCHAIN_PREFIX ROMODULAR_AVR_TOOLCHAIN_PREFIX
    STRING "avr" "AVR-GCC compiler prefix")
dspcore_forward_toolchain_cache(DSPCORE_AVR_MCU ROMODULAR_AVR_MCU
    STRING "" "AVR MCU name accepted by -mmcu")
dspcore_forward_toolchain_cache(DSPCORE_AVR_ARCHITECTURE ROMODULAR_AVR_ARCHITECTURE
    STRING "avr" "AVR architecture used as CMAKE_SYSTEM_PROCESSOR metadata")
dspcore_forward_toolchain_cache(DSPCORE_AVR_ADDITIONAL_FLAGS ROMODULAR_AVR_ADDITIONAL_FLAGS
    STRING "" "Additional flags shared by C and C++")

set(DSPCORE_ROMODULAR_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../../tools/RoModularBuild")
set(DSPCORE_ROMODULAR_AVR_TOOLCHAIN
    "${DSPCORE_ROMODULAR_ROOT}/cmake/toolchains/avr-gcc.cmake")
if(NOT EXISTS "${DSPCORE_ROMODULAR_AVR_TOOLCHAIN}")
    message(FATAL_ERROR
        "RoModularBuild is not initialized; run "
        "'git submodule update --init --recursive'")
endif()

include("${DSPCORE_ROMODULAR_AVR_TOOLCHAIN}")
