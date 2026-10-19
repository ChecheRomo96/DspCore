dspcore_add_test(DspCoreInterpolationTests ArithmeticTests.cc)
if(DSPCORE_LUT)
    dspcore_add_test(DspCoreLookupTests LookupTests.cc)
endif()
