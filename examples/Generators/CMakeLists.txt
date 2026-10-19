add_subdirectory(Periodic)
add_subdirectory(Shaping)
add_subdirectory(Envelopes)
add_subdirectory(Noise)
add_subdirectory(Windows)

if(DSPCORE_LUT AND DSPCORE_INTERPOLATION)
    add_subdirectory(LUTEnvelope)
endif()
