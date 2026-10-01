set(DSPCORE_AVR_MCU "atmega328p" CACHE STRING "Target AVR MCU" FORCE)
set(DSPCORE_AVR_ARCHITECTURE "avr5" CACHE STRING
    "Target AVR architecture" FORCE)

include("${CMAKE_CURRENT_LIST_DIR}/../avr-gcc.cmake")
