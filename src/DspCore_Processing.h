#ifndef DSPCORE_PROCESSING_TOP_LEVEL_H
#define DSPCORE_PROCESSING_TOP_LEVEL_H

#include <DspCore_BuildSettings.h>

#if __has_include(<DspCore/Processing.h>)
    #ifndef DSPCORE_PROCESSING
        #define DSPCORE_PROCESSING
    #endif
    #include <DspCore/Processing.h>
#endif

#endif
