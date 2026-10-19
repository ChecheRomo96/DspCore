#ifndef DSPCORE_GENERATORS_H
#define DSPCORE_GENERATORS_H
#include <DspCore/Generators/Periodic.h>
#include <DspCore/Generators/Shaping.h>
#include <DspCore/Generators/Envelopes.h>
#include <DspCore/Generators/Noise.h>
#include <DspCore/Generators/Windows.h>
#if __has_include(<DspCore/Generators/LUTSegment.h>) && __has_include(<DspCore/LUT.h>) && __has_include(<DspCore/Interpolation/Lookup.h>)
    #include <DspCore/Generators/LUTSegment.h>
#endif
#endif
