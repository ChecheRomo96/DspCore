cmake_minimum_required(VERSION 3.25)

project(DspCore VERSION 0.1.1 LANGUAGES C CXX)

set(DSPCORE_REQUIRED_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

set(DSPCORE_ROOT_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}")
set(DSPCORE_PLATFORM "desktop" CACHE STRING "DspCore target platform identifier")
set(DSPCORE_EXPECTED_SYSTEM_PROCESSOR "" CACHE STRING
    "Regular expression for the expected target processor")

file(STRINGS "${DSPCORE_ROOT_DIRECTORY}/library.properties"
    DSPCORE_ARDUINO_VERSION_LINE REGEX "^version=" LIMIT_COUNT 1)
string(REGEX REPLACE "^version=" "" DSPCORE_ARDUINO_VERSION
    "${DSPCORE_ARDUINO_VERSION_LINE}")
if(NOT DSPCORE_ARDUINO_VERSION STREQUAL PROJECT_VERSION)
    message(FATAL_ERROR
        "Version mismatch: CMake declares ${PROJECT_VERSION}, but "
        "library.properties declares ${DSPCORE_ARDUINO_VERSION}")
endif()

file(STRINGS "${DSPCORE_ROOT_DIRECTORY}/src/DspCore_BuildSettings.h"
    DSPCORE_SOURCE_VERSION_LINE
    REGEX "^[ \t]*#define[ \t]+DSPCORE_VERSION[ \t]+\"[^\"]+\""
    LIMIT_COUNT 1)
if(NOT DSPCORE_SOURCE_VERSION_LINE MATCHES "\"([^\"]+)\"")
    message(FATAL_ERROR "DspCore_BuildSettings.h does not declare DSPCORE_VERSION")
endif()
if(NOT CMAKE_MATCH_1 STREQUAL PROJECT_VERSION)
    message(FATAL_ERROR
        "Version mismatch: CMake declares ${PROJECT_VERSION}, but "
        "DspCore_BuildSettings.h declares ${CMAKE_MATCH_1}")
endif()

if(DSPCORE_EXPECTED_SYSTEM_PROCESSOR AND
   NOT CMAKE_SYSTEM_PROCESSOR MATCHES "${DSPCORE_EXPECTED_SYSTEM_PROCESSOR}")
    message(FATAL_ERROR
        "Preset '${DSPCORE_PLATFORM}' expects processor "
        "'${DSPCORE_EXPECTED_SYSTEM_PROCESSOR}', but CMake detected "
        "'${CMAKE_SYSTEM_PROCESSOR}'")
endif()

include(cmake/DspCoreFoundation.cmake)

message(STATUS "Building DspCore Version = ${PROJECT_VERSION}")
message(STATUS "Required C++ standard = C++${DSPCORE_REQUIRED_CXX_STANDARD}")
message(STATUS "System      = ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor   = ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "CXX ID      = ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Foundation  = ${Foundation_VERSION} (${Foundation_PLATFORM}) from ${DSPCORE_FOUNDATION_SOURCE}")

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(cmake/DspCoreHelpers.cmake)
include(CMakeOptions.cmake)

if(DSPCORE_COVERAGE)
    message(STATUS "Enabling code coverage")
    set(DSPCORE_COVERAGE_FLAGS "--coverage -O0 -g")
    string(APPEND CMAKE_C_FLAGS_DEBUG " ${DSPCORE_COVERAGE_FLAGS}")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " ${DSPCORE_COVERAGE_FLAGS}")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " ${DSPCORE_COVERAGE_FLAGS}")
endif()

add_subdirectory(src)
dspcore_add_macro(PUBLIC DSPCORE_VERSION="${PROJECT_VERSION}")

if(DSPCORE_EXAMPLES)
    add_subdirectory(examples)
endif()

add_custom_target(DspCoreExportArtifacts)
add_dependencies(DspCoreExportArtifacts DspCore)
if(TARGET DspCoreExamples)
    add_dependencies(DspCoreExportArtifacts DspCoreExamples)
endif()

if(DSPCORE_DOCS)
    dspcore_add_dox("${DSPCORE_ROOT_DIRECTORY}/README.md")
    include(cmake/BuildDocs.cmake)
endif()

if(DSPCORE_TESTING)
    include(cmake/BuildTests.cmake)
    add_subdirectory(tests)
    dspcore_enable_warnings(DspCore)
endif()

configure_package_config_file(
    "${DSPCORE_ROOT_DIRECTORY}/cmake/DspCoreConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DspCore")

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)

configure_file(
    "${DSPCORE_ROOT_DIRECTORY}/cmake/DspCoreBuildInfo.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreBuildInfo.cmake"
    @ONLY)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/DspCoreBuildInfo.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/DspCore"
    CONFIGURATIONS Release)
