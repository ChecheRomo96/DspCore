dspcore_add_test(DspCoreGeneratorTests GeneratorTests.cc)
if(DSPCORE_LUT AND DSPCORE_INTERPOLATION)
    dspcore_add_test(DspCoreLUTSegmentTests LUTSegmentTests.cc)
endif()
