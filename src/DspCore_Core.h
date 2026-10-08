#ifndef DSPCORE_CORE_TOP_LEVEL_H
#define DSPCORE_CORE_TOP_LEVEL_H

#include <DspCore_BuildSettings.h>

#if __has_include(<DspCore/Core.h>)
    #ifndef DSPCORE_CORE
        #define DSPCORE_CORE
    #endif

    #include <DspCore/Core.h>
#endif

#endif // DSPCORE_CORE_TOP_LEVEL_H
