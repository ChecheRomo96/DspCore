function(dspcore_add_macro visibility)
    target_compile_definitions(DspCore ${visibility} ${ARGN})
    set_property(TARGET DspCore APPEND PROPERTY DSPCORE_DOXYGEN_PREDEFS ${ARGN})
endfunction()

function(dspcore_add_dox)
    set_property(TARGET DspCore APPEND PROPERTY DSPCORE_DOXYGEN_INPUTS ${ARGN})
endfunction()

# Agreed warning set for DspCore's own targets when testing, matching
# Foundation. Warnings are errors; pass --compile-no-warning-as-error to cmake
# to relax locally.
function(dspcore_enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /permissive-)
    else()
        target_compile_options(${target} PRIVATE
            -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wsign-conversion
            -Wold-style-cast -Wnon-virtual-dtor -Woverloaded-virtual
        )
    endif()
    set_target_properties(${target} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
endfunction()

function(dspcore_add_test test_target)
    add_executable(${test_target}
        ${ARGN}
    )

    target_link_libraries(${test_target}
        PRIVATE
            DspCore::DspCore
            GTest::gtest_main
    )

    # Test sources spell Unicode accidentals as escapes in narrow literals,
    # which need a UTF-8 execution character set on MSVC.
    target_compile_options(${test_target} PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
    dspcore_enable_warnings(${test_target})

    gtest_discover_tests(${test_target}
        TEST_PREFIX "${test_target}."
        DISCOVERY_MODE PRE_TEST
        PROPERTIES LABELS "DspCore"
    )

    set_property(TARGET DspCore APPEND PROPERTY DSPCORE_TEST_TARGETS ${test_target})
endfunction()

function(dspcore_stage_headers)
    foreach(HEADER ${ARGV})
        file(RELATIVE_PATH REL_HEADER "${DSPCORE_SRC_DIRECTORY}" "${HEADER}")
        get_filename_component(REL_DIR "${REL_HEADER}" DIRECTORY)
        file(MAKE_DIRECTORY "${DSPCORE_BUILD_INCLUDE_DIR}/${REL_DIR}")

        configure_file(
            "${HEADER}"
            "${DSPCORE_BUILD_INCLUDE_DIR}/${REL_HEADER}"
            COPYONLY
        )
    endforeach()
endfunction()
