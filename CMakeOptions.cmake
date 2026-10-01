option(DSPCORE_EXAMPLES "Enable building examples" OFF)
option(DSPCORE_TESTING "Enable unit testing" OFF)
option(DSPCORE_DOCS "Generate API documentation using Doxygen" OFF)
option(DSPCORE_FULL_BUILD "Enable every DspCore module" OFF)
option(DSPCORE_COVERAGE "Enable coverage instrumentation" OFF)

option(DSPCORE_CORE "Enable DspCore::Core" ON)

if(DSPCORE_FULL_BUILD)
    set(DSPCORE_CORE ON CACHE BOOL "Enable DspCore::Core" FORCE)
endif()
