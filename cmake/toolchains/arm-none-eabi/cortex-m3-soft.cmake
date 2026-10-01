set(DSPCORE_ARM_CPU "cortex-m3" CACHE STRING "Target Arm CPU" FORCE)
set(DSPCORE_FLOAT_ABI "soft" CACHE STRING "Target floating-point ABI" FORCE)
set(DSPCORE_FPU "" CACHE STRING "Target FPU name" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/../arm-none-eabi.cmake")
