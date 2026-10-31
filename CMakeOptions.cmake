option(DSPCORE_EXAMPLES "Enable building examples" OFF)
option(DSPCORE_TESTING "Enable unit testing" OFF)
option(DSPCORE_DOCS "Generate API documentation using Doxygen" OFF)
option(DSPCORE_FULL_BUILD "Enable every DspCore module" OFF)
option(DSPCORE_COVERAGE "Enable coverage instrumentation" OFF)

option(DSPCORE_CORE "Enable DspCore::Core" ON)

if(DSPCORE_FULL_BUILD)
    set(DSPCORE_CORE ON CACHE BOOL "Enable DspCore::Core" FORCE)
endif()

option(DSPCORE_LUT "Enable DspCore::LUT" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_LUT ON CACHE BOOL "Enable DspCore::LUT" FORCE)
endif()

option(DSPCORE_INTERPOLATION "Enable DspCore::Interpolation" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_INTERPOLATION ON CACHE BOOL "Enable DspCore::Interpolation" FORCE)
endif()

option(DSPCORE_GENERATORS "Enable DspCore::Generators" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_GENERATORS ON CACHE BOOL "Enable DspCore::Generators" FORCE)
endif()

option(DSPCORE_PROCESSING "Enable DspCore::Processing" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_PROCESSING ON CACHE BOOL "Enable DspCore::Processing" FORCE)
endif()

option(DSPCORE_FILTERS "Enable DspCore::Filters" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_FILTERS ON CACHE BOOL "Enable DspCore::Filters" FORCE)
endif()
if(DSPCORE_FILTERS)
    # FIR and IIR use the typed SampleBlock view contract.
    set(DSPCORE_PROCESSING ON CACHE BOOL "Enable DspCore::Processing" FORCE)
endif()

option(DSPCORE_ANALYSIS "Enable DspCore::Analysis" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_ANALYSIS ON CACHE BOOL "Enable DspCore::Analysis" FORCE)
endif()

option(DSPCORE_DYNAMICS "Enable DspCore::Dynamics" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_DYNAMICS ON CACHE BOOL "Enable DspCore::Dynamics" FORCE)
endif()

option(DSPCORE_TRANSFORMS "Enable DspCore::Transforms" ON)
if(DSPCORE_FULL_BUILD)
    set(DSPCORE_TRANSFORMS ON CACHE BOOL "Enable DspCore::Transforms" FORCE)
endif()
