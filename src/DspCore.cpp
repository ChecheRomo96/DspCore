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
// included. Polymorphic ProcessorBase implementations retain size budgets but
// are intentionally not value types.
#define DSPCORE_CHECK_VALUE_TYPE(Type, MaximumBytes)                                  \
    static_assert(sizeof(Type) <= (MaximumBytes), #Type " exceeds its size budget"); \
    static_assert(__is_trivially_copyable(Type), #Type " must be trivially copyable")

#if defined(DSPCORE_INTERPOLATION) && defined(DSPCORE_LUT)
DSPCORE_CHECK_VALUE_TYPE(DspCore::Interpolation::Boundary, 1);
#endif


#if defined(DSPCORE_FILTERS)
namespace {
// ARM can align double to eight bytes while size_t occupies four bytes.
// Include the ABI's tail padding without allowing extra processor state.
constexpr size_t AlignedSizeBudget(size_t bytes,size_t alignment) noexcept {
    return ((bytes+alignment-1)/alignment)*alignment;
}
}
using DspCoreFirFloatBudget=DspCore::Filters::FIR<float,8>;
using DspCoreFirDoubleBudget=DspCore::Filters::FIR<double,8>;
static_assert(sizeof(DspCoreFirFloatBudget)<=
    AlignedSizeBudget(16*sizeof(float)+2*sizeof(size_t)+sizeof(void*),alignof(DspCoreFirFloatBudget)),
    "DspCoreFirFloatBudget exceeds its size budget");
static_assert(sizeof(DspCoreFirDoubleBudget)<=
    AlignedSizeBudget(16*sizeof(double)+2*sizeof(size_t)+sizeof(void*),alignof(DspCoreFirDoubleBudget)),
    "DspCoreFirDoubleBudget exceeds its size budget");
DSPCORE_CHECK_VALUE_TYPE(DspCore::Filters::Biquad::Coefficients<float>, 6*sizeof(float));
DSPCORE_CHECK_VALUE_TYPE(DspCore::Filters::Biquad::Coefficients<double>, 6*sizeof(double));
static_assert(sizeof(DspCore::Filters::IIR<float>)<=
    AlignedSizeBudget(10*sizeof(float)+sizeof(void*),alignof(DspCore::Filters::IIR<float>)),
    "DspCore::Filters::IIR<float> exceeds its size budget");
static_assert(sizeof(DspCore::Filters::IIR<double>)<=
    AlignedSizeBudget(10*sizeof(double)+sizeof(void*),alignof(DspCore::Filters::IIR<double>)),
    "DspCore::Filters::IIR<double> exceeds its size budget");
#endif
#if defined(DSPCORE_ANALYSIS)
DSPCORE_CHECK_VALUE_TYPE(DspCore::Analysis::EnvelopeFollower<float>, 3*sizeof(float));
DSPCORE_CHECK_VALUE_TYPE(DspCore::Analysis::EnvelopeFollower<double>, 3*sizeof(double));
#endif

#undef DSPCORE_CHECK_VALUE_TYPE
