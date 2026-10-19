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
