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


if(DSPCORE_FILTERS)
    add_subdirectory(Filters)
endif()

if(DSPCORE_ANALYSIS)
    add_subdirectory(Analysis)
endif()

if(DSPCORE_PROCESSING)
    add_subdirectory(Processing)
endif()
