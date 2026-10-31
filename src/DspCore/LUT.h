#ifndef DSPCORE_LUT_H
#define DSPCORE_LUT_H
#include <DspCore/LUT/LUT1D.h>
#include <DspCore/LUT/LUT2D.h>
#include <DspCore/LUT/LUT3D.h>
#if __has_include(<DspCore/LUT/Sampling.h>) && __has_include(<DspCore/Interpolation/Arithmetic.h>)
    #include <DspCore/LUT/Sampling.h>
#endif
#endif
