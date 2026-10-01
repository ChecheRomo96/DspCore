# DspCore compatibility wrapper for RoModularBuild.
# Target profiles continue to set the established DSPCORE_* variables.

include("${CMAKE_CURRENT_LIST_DIR}/DspCoreRoModularCompatibility.cmake")

dspcore_forward_toolchain_cache(DSPCORE_ARM_TOOLCHAIN_ROOT ROMODULAR_ARM_TOOLCHAIN_ROOT
    PATH "" "Optional GNU Arm Embedded installation root")
dspcore_forward_toolchain_cache(DSPCORE_ARM_TOOLCHAIN_PREFIX ROMODULAR_ARM_TOOLCHAIN_PREFIX
    STRING "arm-none-eabi" "GNU Arm Embedded compiler prefix")
dspcore_forward_toolchain_cache(DSPCORE_ARM_CPU ROMODULAR_ARM_CPU
    STRING "cortex-m3" "Target Arm CPU")
dspcore_forward_toolchain_cache(DSPCORE_FLOAT_ABI ROMODULAR_FLOAT_ABI
    STRING "soft" "Target floating-point ABI")
dspcore_forward_toolchain_cache(DSPCORE_FPU ROMODULAR_FPU
    STRING "" "Target FPU name")
dspcore_forward_toolchain_cache(DSPCORE_ARM_ADDITIONAL_FLAGS ROMODULAR_ARM_ADDITIONAL_FLAGS
    STRING "" "Additional flags shared by C and C++")
dspcore_forward_toolchain_cache(DSPCORE_ARM_SYSROOT ROMODULAR_ARM_SYSROOT
    PATH "" "Optional target sysroot containing the C runtime headers and libraries")

set(DSPCORE_ROMODULAR_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../../tools/RoModularBuild")
set(DSPCORE_ROMODULAR_ARM_TOOLCHAIN
    "${DSPCORE_ROMODULAR_ROOT}/cmake/toolchains/arm-none-eabi.cmake")
if(NOT EXISTS "${DSPCORE_ROMODULAR_ARM_TOOLCHAIN}")
    message(FATAL_ERROR
        "RoModularBuild is not initialized; run "
        "'git submodule update --init --recursive'")
endif()

include("${DSPCORE_ROMODULAR_ARM_TOOLCHAIN}")
