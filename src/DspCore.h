#ifndef DSPCORE_H
#define DSPCORE_H

#include <DspCore_BuildSettings.h>

#if __has_include(<DspCore_Core.h>)
    #include <DspCore_Core.h>
#endif

#if __has_include(<DspCore_LUT.h>)
    #include <DspCore_LUT.h>
#endif

#if __has_include(<DspCore_Interpolation.h>)
    #include <DspCore_Interpolation.h>
#endif

#if __has_include(<DspCore_Generators.h>)
    #include <DspCore_Generators.h>
#endif


#if __has_include(<DspCore_Filters.h>)
    #include <DspCore_Filters.h>
#endif

#if __has_include(<DspCore_Analysis.h>)
    #include <DspCore_Analysis.h>
#endif

#if __has_include(<DspCore_Dynamics.h>)
    #include <DspCore_Dynamics.h>
#endif

#if __has_include(<DspCore_Transforms.h>)
    #include <DspCore_Transforms.h>
#endif

#if __has_include(<DspCore_Processing.h>)
    #include <DspCore_Processing.h>
#endif

#endif // DSPCORE_H
