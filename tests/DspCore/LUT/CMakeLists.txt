dspcore_add_test(DspCoreLUTTests LUTTests.cc)
if(DSPCORE_INTERPOLATION)
    dspcore_add_test(DspCoreLUTSamplingTests SamplingTests.cc)
endif()
