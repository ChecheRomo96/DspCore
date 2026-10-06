# Resolves the Foundation dependency and defines Foundation::Foundation.
#
# Resolution order:
#   0. A Foundation::Foundation target already defined by a parent project.
#   1. DSPCORE_FOUNDATION_PREFIX (cache or environment): an explicit package
#      prefix. Failing to use it is a fatal error.
#   2. ../Foundation/dist/<DSPCORE_PLATFORM>: a sibling export. Skipped with a
#      warning when it is incompatible.
#   3. Normal find_package() search: Foundation_DIR, CMAKE_PREFIX_PATH and
#      system locations.
#   4. DSPCORE_FETCH_FOUNDATION (default ON):
#      a. The GitHub Release package for tag v<DSPCORE_FOUNDATION_VERSION> and
#         this platform, verified against its published SHA-256.
#      b. The Foundation sources at that tag, built as part of DspCore (used for
#         platforms without a Release package, such as AVR and Arm, and when
#         FETCHCONTENT_SOURCE_DIR_FOUNDATION points at a local working copy).
#
# Downloads live in ${CMAKE_BINARY_DIR}/_deps and are reused across
# reconfigurations.
#
# Outputs: Foundation_VERSION, Foundation_PLATFORM, DSPCORE_FOUNDATION_SOURCE
# and the internal cache entry DSPCORE_FOUNDATION_RESOLVED_PREFIX (empty when
# Foundation is built from source and installed alongside DspCore).

set(DSPCORE_FOUNDATION_VERSION "2.0.0")
set(DSPCORE_FOUNDATION_REPOSITORY "ChecheRomo96/Foundation" CACHE STRING
    "GitHub repository (owner/name) used to fetch Foundation")
option(DSPCORE_FETCH_FOUNDATION
    "Fetch Foundation from GitHub when no local package is found" ON)

set(DSPCORE_FOUNDATION_SOURCE "")
# Foundation_DIR as given by the user, before candidate searches reset it.
set(DSPCORE_USER_FOUNDATION_DIR "${Foundation_DIR}")

# Restores Foundation_DIR to the user's value so candidate searches never
# leave their own result in the cache.
macro(dspcore_restore_foundation_dir)
    if(DSPCORE_USER_FOUNDATION_DIR AND
       NOT DSPCORE_USER_FOUNDATION_DIR MATCHES "-NOTFOUND$")
        set(Foundation_DIR "${DSPCORE_USER_FOUNDATION_DIR}" CACHE PATH
            "Directory containing FoundationConfig.cmake" FORCE)
    else()
        unset(Foundation_DIR CACHE)
    endif()
endmacro()

function(dspcore_find_foundation_package prefix)
    # A cached Foundation_DIR takes precedence over PATHS, so search each
    # candidate prefix from scratch.
    unset(Foundation_DIR CACHE)
    find_package(Foundation ${DSPCORE_FOUNDATION_VERSION} CONFIG QUIET
        PATHS "${prefix}"
        NO_DEFAULT_PATH
        NO_CMAKE_FIND_ROOT_PATH)
    dspcore_restore_foundation_dir()
    if(TARGET Foundation::Foundation)
        set(Foundation_VERSION "${Foundation_VERSION}" PARENT_SCOPE)
        set(Foundation_PLATFORM "${Foundation_PLATFORM}" PARENT_SCOPE)
        set(Foundation_CXX_STANDARD "${Foundation_CXX_STANDARD}" PARENT_SCOPE)
    endif()
endfunction()

# Downloads and extracts the Release package; sets DSPCORE_FOUNDATION_PACKAGE_PREFIX
# in the caller on success.
function(dspcore_download_foundation_package)
    set(DSPCORE_FOUNDATION_PACKAGE_PREFIX "" PARENT_SCOPE)

    set(name "Foundation-${DSPCORE_FOUNDATION_VERSION}-${DSPCORE_PLATFORM}")
    if(DSPCORE_PLATFORM MATCHES "^windows_")
        set(archive "${name}.zip")
    else()
        set(archive "${name}.tar.gz")
    endif()

    set(root "${CMAKE_BINARY_DIR}/_deps/foundation-package")
    set(prefix "${root}/${name}")
    if(EXISTS "${prefix}/lib/cmake/Foundation/FoundationConfig.cmake")
        set(DSPCORE_FOUNDATION_PACKAGE_PREFIX "${prefix}" PARENT_SCOPE)
        return()
    endif()

    set(url "https://github.com/${DSPCORE_FOUNDATION_REPOSITORY}/releases/download/v${DSPCORE_FOUNDATION_VERSION}")
    file(MAKE_DIRECTORY "${root}")

    file(DOWNLOAD "${url}/${archive}.sha256" "${root}/${archive}.sha256"
        STATUS status)
    list(GET status 0 code)
    if(NOT code EQUAL 0)
        file(REMOVE "${root}/${archive}.sha256")
        message(STATUS
            "Foundation Release ${DSPCORE_FOUNDATION_VERSION} has no package for "
            "'${DSPCORE_PLATFORM}'")
        return()
    endif()

    file(STRINGS "${root}/${archive}.sha256" checksum_line LIMIT_COUNT 1)
    if(NOT checksum_line MATCHES "^([0-9a-fA-F]+)")
        message(FATAL_ERROR "Malformed checksum file for ${archive}")
    endif()
    set(checksum "${CMAKE_MATCH_1}")

    message(STATUS "Downloading ${archive}")
    # Do not pass EXPECTED_HASH to file(DOWNLOAD): CMake records a configure
    # error before STATUS can be inspected when the asset is unavailable.
    # Download first, fall back to sources on transport errors, and verify the
    # published digest explicitly before extracting anything.
    file(DOWNLOAD "${url}/${archive}" "${root}/${archive}"
        STATUS status)
    list(GET status 0 code)
    if(NOT code EQUAL 0)
        list(GET status 1 reason)
        file(REMOVE "${root}/${archive}" "${root}/${archive}.sha256")
        message(STATUS
            "Foundation Release package ${archive} is unavailable (${reason}); "
            "falling back to sources")
        return()
    endif()

    file(SHA256 "${root}/${archive}" actual_checksum)
    string(TOLOWER "${checksum}" checksum)
    string(TOLOWER "${actual_checksum}" actual_checksum)
    if(NOT actual_checksum STREQUAL checksum)
        file(REMOVE "${root}/${archive}" "${root}/${archive}.sha256")
        message(FATAL_ERROR
            "SHA-256 mismatch for ${archive}: expected ${checksum}, got "
            "${actual_checksum}")
    endif()

    file(ARCHIVE_EXTRACT INPUT "${root}/${archive}" DESTINATION "${root}")
    file(REMOVE "${root}/${archive}" "${root}/${archive}.sha256")

    if(NOT EXISTS "${prefix}/lib/cmake/Foundation/FoundationConfig.cmake")
        message(FATAL_ERROR "${archive} does not contain a Foundation package")
    endif()
    set(DSPCORE_FOUNDATION_PACKAGE_PREFIX "${prefix}" PARENT_SCOPE)
endfunction()

# 0. Target provided by a parent project.
if(TARGET Foundation::Foundation)
    set(DSPCORE_FOUNDATION_SOURCE "parent project")
    set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "")
endif()

# 1. Explicit prefix.
if(NOT TARGET Foundation::Foundation AND NOT DEFINED DSPCORE_FOUNDATION_PREFIX AND
   NOT "$ENV{DSPCORE_FOUNDATION_PREFIX}" STREQUAL "")
    set(DSPCORE_FOUNDATION_PREFIX "$ENV{DSPCORE_FOUNDATION_PREFIX}")
endif()
set(DSPCORE_FOUNDATION_PREFIX "${DSPCORE_FOUNDATION_PREFIX}" CACHE PATH
    "Foundation package prefix for the same platform and ABI")

if(NOT TARGET Foundation::Foundation AND NOT DSPCORE_FOUNDATION_PREFIX STREQUAL "")
    dspcore_find_foundation_package("${DSPCORE_FOUNDATION_PREFIX}")
    if(NOT TARGET Foundation::Foundation)
        message(FATAL_ERROR
            "DSPCORE_FOUNDATION_PREFIX='${DSPCORE_FOUNDATION_PREFIX}' does not contain "
            "a Foundation ${DSPCORE_FOUNDATION_VERSION}+ (2.x) package.")
    endif()
    set(DSPCORE_FOUNDATION_SOURCE "prefix")
    set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "${DSPCORE_FOUNDATION_PREFIX}")
endif()

# 2. Sibling export.
if(NOT TARGET Foundation::Foundation)
    set(sibling_prefix "${DSPCORE_ROOT_DIRECTORY}/../Foundation/dist/${DSPCORE_PLATFORM}")
    get_filename_component(sibling_prefix "${sibling_prefix}" ABSOLUTE)
    if(EXISTS "${sibling_prefix}/lib/cmake/Foundation/FoundationConfig.cmake")
        dspcore_find_foundation_package("${sibling_prefix}")
        if(TARGET Foundation::Foundation)
            set(DSPCORE_FOUNDATION_SOURCE "sibling export")
            set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "${sibling_prefix}")
        else()
            message(WARNING
                "Ignoring ${sibling_prefix}: it is not a compatible Foundation "
                "${DSPCORE_FOUNDATION_VERSION}+ (2.x) package. Re-export it or "
                "let DspCore fetch Foundation.")
        endif()
    endif()
endif()

# 3. Normal package search.
if(NOT TARGET Foundation::Foundation)
    dspcore_restore_foundation_dir()
    find_package(Foundation ${DSPCORE_FOUNDATION_VERSION} CONFIG QUIET)
    if(TARGET Foundation::Foundation)
        set(DSPCORE_FOUNDATION_SOURCE "${Foundation_DIR}")
        get_filename_component(DSPCORE_FOUNDATION_RESOLVED_PREFIX
            "${Foundation_DIR}/../../.." ABSOLUTE)
    else()
        dspcore_restore_foundation_dir()
    endif()
endif()

# 4. Fetch from GitHub.
if(NOT TARGET Foundation::Foundation AND DSPCORE_FETCH_FOUNDATION)
    # A local source override means "build this working copy", so skip the
    # prebuilt Release package.
    if(FETCHCONTENT_SOURCE_DIR_FOUNDATION)
        set(DSPCORE_FOUNDATION_PACKAGE_PREFIX "")
    else()
        dspcore_download_foundation_package()
    endif()
    if(NOT DSPCORE_FOUNDATION_PACKAGE_PREFIX STREQUAL "")
        dspcore_find_foundation_package("${DSPCORE_FOUNDATION_PACKAGE_PREFIX}")
        if(NOT TARGET Foundation::Foundation)
            message(FATAL_ERROR
                "The downloaded Foundation package at "
                "${DSPCORE_FOUNDATION_PACKAGE_PREFIX} could not be loaded.")
        endif()
        set(DSPCORE_FOUNDATION_SOURCE "GitHub Release v${DSPCORE_FOUNDATION_VERSION}")
        set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "${DSPCORE_FOUNDATION_PACKAGE_PREFIX}")
    else()
        include(FetchContent)
        message(STATUS "Fetching Foundation sources at v${DSPCORE_FOUNDATION_VERSION}")
        FetchContent_Declare(Foundation
            GIT_REPOSITORY "https://github.com/${DSPCORE_FOUNDATION_REPOSITORY}.git"
            GIT_TAG "v${DSPCORE_FOUNDATION_VERSION}"
            GIT_SHALLOW TRUE)
        FetchContent_MakeAvailable(Foundation)
        if(NOT TARGET Foundation::Foundation)
            message(FATAL_ERROR
                "Foundation v${DSPCORE_FOUNDATION_VERSION} sources do not define "
                "Foundation::Foundation.")
        endif()
        # A source build shares DspCore's toolchain, so only the version needs
        # checking; library.properties carries it for every Foundation release.
        file(STRINGS "${foundation_SOURCE_DIR}/library.properties"
            foundation_version_line REGEX "^version=" LIMIT_COUNT 1)
        string(REGEX REPLACE "^version=" "" Foundation_VERSION
            "${foundation_version_line}")
        string(REGEX MATCH "^[0-9]+" foundation_major "${Foundation_VERSION}")
        string(REGEX MATCH "^[0-9]+" required_major "${DSPCORE_FOUNDATION_VERSION}")
        if(Foundation_VERSION VERSION_LESS DSPCORE_FOUNDATION_VERSION OR
           NOT foundation_major STREQUAL required_major)
            message(FATAL_ERROR
                "Foundation sources at ${foundation_SOURCE_DIR} are version "
                "'${Foundation_VERSION}', but DspCore requires "
                "${DSPCORE_FOUNDATION_VERSION}+ (${required_major}.x).")
        endif()
        set(Foundation_PLATFORM "${DSPCORE_PLATFORM}")
        if(FETCHCONTENT_SOURCE_DIR_FOUNDATION)
            set(DSPCORE_FOUNDATION_SOURCE "sources at ${FETCHCONTENT_SOURCE_DIR_FOUNDATION}")
        else()
            set(DSPCORE_FOUNDATION_SOURCE "sources at v${DSPCORE_FOUNDATION_VERSION}")
        endif()
        set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "")
    endif()
endif()

if(NOT TARGET Foundation::Foundation)
    message(FATAL_ERROR
        "DspCore requires Foundation ${DSPCORE_FOUNDATION_VERSION}+ (2.x) for the "
        "same platform and ABI. Set DSPCORE_FOUNDATION_PREFIX to a Foundation "
        "package, export ../Foundation/dist/${DSPCORE_PLATFORM}, or enable "
        "DSPCORE_FETCH_FOUNDATION.")
endif()

set(DSPCORE_FOUNDATION_RESOLVED_PREFIX "${DSPCORE_FOUNDATION_RESOLVED_PREFIX}" CACHE
    INTERNAL "Foundation package prefix used by this build")

if(DEFINED Foundation_CXX_STANDARD AND
   NOT Foundation_CXX_STANDARD STREQUAL "" AND
   Foundation_CXX_STANDARD GREATER DSPCORE_REQUIRED_CXX_STANDARD)
    message(FATAL_ERROR
        "Foundation requires C++${Foundation_CXX_STANDARD}, but DspCore is "
        "configured for C++${DSPCORE_REQUIRED_CXX_STANDARD}")
endif()

if(NOT DSPCORE_PLATFORM STREQUAL "documentation" AND
   DEFINED Foundation_PLATFORM AND
   NOT Foundation_PLATFORM STREQUAL "" AND
   NOT Foundation_PLATFORM STREQUAL DSPCORE_PLATFORM)
    message(FATAL_ERROR
        "DspCore preset '${DSPCORE_PLATFORM}' cannot consume Foundation package "
        "'${Foundation_PLATFORM}'. Use a package for the same platform.")
endif()
