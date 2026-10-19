#include <DspCore.h>
#include <float.h>

// SPEC-NUM-1: compiled for each target, including source-only Arduino builds.
static_assert(FLT_RADIX == 2, "DspCore requires binary floating-point");
static_assert(FLT_MANT_DIG >= 24, "DspCore requires binary32 significand precision");
static_assert(FLT_MAX_EXP >= 128, "DspCore requires binary32 upper exponent range");
static_assert(FLT_MIN_EXP < -124, "DspCore requires binary32 lower exponent range");
static_assert(DBL_MANT_DIG >= FLT_MANT_DIG, "double must preserve float precision");
static_assert(DBL_MAX_EXP >= FLT_MAX_EXP, "double must preserve float upper range");
static_assert(DBL_MIN_EXP <= FLT_MIN_EXP, "double must preserve float lower range");

// SPEC-EMB-1..2: size budgets and trivial copyability of DspCore value types
// are checked here, when the library is built for every target, AVR and Arm
// included. Value types are added by later reconstruction phases.
#define DSPCORE_CHECK_VALUE_TYPE(Type, MaximumBytes)                                  \
    static_assert(sizeof(Type) <= (MaximumBytes), #Type " exceeds its size budget"); \
    static_assert(__is_trivially_copyable(Type), #Type " must be trivially copyable")

#if defined(DSPCORE_INTERPOLATION) && defined(DSPCORE_LUT)
DSPCORE_CHECK_VALUE_TYPE(DspCore::Interpolation::Boundary, 1);
#endif

#undef DSPCORE_CHECK_VALUE_TYPE
