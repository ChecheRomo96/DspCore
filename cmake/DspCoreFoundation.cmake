# Resolves the Foundation source dependency and defines Foundation::Foundation.
#
# The embedding project owns the dependency graph. It may provide Foundation
# before adding DspCore; otherwise DspCore fetches the pinned Foundation source
# revision. DspCoreConfig.cmake.in retains find_dependency(Foundation) for
# exported Release packages.

set(DSPCORE_FOUNDATION_VERSION "0.0.1")
set(DSPCORE_FOUNDATION_REPOSITORY "ChecheRomo96/Foundation" CACHE STRING
    "GitHub repository (owner/name) used to fetch Foundation")
option(DSPCORE_FETCH_FOUNDATION
    "Fetch Foundation source when a parent project does not provide it" ON)

set(DSPCORE_FOUNDATION_SOURCE "")
set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "")

if(TARGET Foundation::Foundation)
    set(DSPCORE_FOUNDATION_SOURCE "parent project")
elseif(NOT DSPCORE_FETCH_FOUNDATION)
    message(FATAL_ERROR
        "DspCore requires a parent-provided Foundation::Foundation target, or "
        "DSPCORE_FETCH_FOUNDATION=ON.")
else()
    include(FetchContent)
    if(NOT DEFINED FOUNDATION_PLATFORM OR FOUNDATION_PLATFORM STREQUAL "")
        set(FOUNDATION_PLATFORM "${DSPCORE_PLATFORM}" CACHE STRING
            "Foundation target platform identifier")
    endif()
    message(STATUS "Fetching Foundation sources at v${DSPCORE_FOUNDATION_VERSION}")
    FetchContent_Declare(Foundation
        GIT_REPOSITORY "https://github.com/${DSPCORE_FOUNDATION_REPOSITORY}.git"
        GIT_TAG "v${DSPCORE_FOUNDATION_VERSION}"
        GIT_SHALLOW TRUE
        GIT_SUBMODULES "")
    FetchContent_MakeAvailable(Foundation)

    if(NOT TARGET Foundation::Foundation)
        message(FATAL_ERROR
            "Foundation v${DSPCORE_FOUNDATION_VERSION} sources do not define "
            "Foundation::Foundation.")
    endif()
    set(Foundation_PLATFORM "${FOUNDATION_PLATFORM}")

    if(FETCHCONTENT_SOURCE_DIR_FOUNDATION)
        set(DSPCORE_FOUNDATION_SOURCE
            "sources at ${FETCHCONTENT_SOURCE_DIR_FOUNDATION}")
    else()
        set(DSPCORE_FOUNDATION_SOURCE
            "sources at v${DSPCORE_FOUNDATION_VERSION}")
    endif()
endif()

if((NOT DEFINED Foundation_VERSION OR Foundation_VERSION STREQUAL "") AND
   DEFINED foundation_SOURCE_DIR AND
   EXISTS "${foundation_SOURCE_DIR}/library.properties")
    file(STRINGS "${foundation_SOURCE_DIR}/library.properties"
        foundation_version_line REGEX "^version=" LIMIT_COUNT 1)
    string(REGEX REPLACE "^version=" "" Foundation_VERSION
        "${foundation_version_line}")
endif()

if(DEFINED Foundation_VERSION AND NOT Foundation_VERSION STREQUAL "" AND
   Foundation_VERSION VERSION_LESS DSPCORE_FOUNDATION_VERSION)
    message(FATAL_ERROR
        "DspCore requires Foundation ${DSPCORE_FOUNDATION_VERSION}+; resolved "
        "${Foundation_VERSION}.")
endif()

set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "" CACHE INTERNAL
    "Foundation package prefix used by this build")
