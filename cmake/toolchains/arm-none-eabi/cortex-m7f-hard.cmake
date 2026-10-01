set(DSPCORE_ARM_CPU "cortex-m7" CACHE STRING "Target Arm CPU" FORCE)
set(DSPCORE_FLOAT_ABI "hard" CACHE STRING "Target floating-point ABI" FORCE)
set(DSPCORE_FPU "fpv5-d16" CACHE STRING "Target FPU name" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/../arm-none-eabi.cmake")
