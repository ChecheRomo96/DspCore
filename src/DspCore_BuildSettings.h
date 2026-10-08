#ifndef DSPCORE_BUILD_SETTINGS_H
#define DSPCORE_BUILD_SETTINGS_H

// The libraries this one depends on. Including them from their src root
// lets the Arduino builder find them from any header of this library.
#include <Foundation_BuildSettings.h>

#ifndef DSPCORE_VERSION
    #define DSPCORE_VERSION "0.1.0"
#endif

#ifndef DSPCORE_CPLUSPLUS
    #if defined(_MSVC_LANG)
        #define DSPCORE_CPLUSPLUS _MSVC_LANG
    #elif defined(__cplusplus)
        #define DSPCORE_CPLUSPLUS __cplusplus
    #else
        #define DSPCORE_CPLUSPLUS 0L
    #endif
#endif

#if !defined(DOXYGEN) && (DSPCORE_CPLUSPLUS < 201703L)
    #error "DspCore requires C++17 or newer"
#endif

#endif // DSPCORE_BUILD_SETTINGS_H
