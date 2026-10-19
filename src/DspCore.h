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

#endif // DSPCORE_H
