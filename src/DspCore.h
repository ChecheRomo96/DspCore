#ifndef DSPCORE_H
#define DSPCORE_H

#include <DspCore_BuildSettings.h>

#if __has_include(<DspCore_Core.h>)
    #include <DspCore_Core.h>
#endif

#if __has_include(<DspCore_Processing.h>)
    #include <DspCore_Processing.h>
#endif

#if __has_include(<DspCore_Filters.h>)
    #include <DspCore_Filters.h>
#endif

#endif // DSPCORE_H
