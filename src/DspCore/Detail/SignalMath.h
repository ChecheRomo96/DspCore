#ifndef DSPCORE_DETAIL_SIGNAL_MATH_H
#define DSPCORE_DETAIL_SIGNAL_MATH_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>
#include <stddef.h>
#include <math.h>
/// @cond DSPCORE_INTERNAL
namespace DspCore { namespace Detail { namespace Signal {
template<typename SampleType>
bool FiniteImpl(const SampleType, cpstd::true_type) noexcept {
    return true;
}

template<typename SampleType>
bool FiniteImpl(const SampleType value, cpstd::false_type) noexcept {
    return value<=cpstd::numeric_limits<SampleType>::max() &&
           value>=-cpstd::numeric_limits<SampleType>::max();
}

template<typename SampleType> bool Finite(const SampleType value) noexcept {
    static_assert(cpstd::is_arithmetic<SampleType>::value,
                  "DspCore signal values must be arithmetic");
    return FiniteImpl(value, typename cpstd::is_integral<SampleType>::type{});
}

template<typename DestinationType,typename SourceType>
bool ConversionFits(const SourceType value, cpstd::true_type) noexcept {
    return value<=static_cast<SourceType>(cpstd::numeric_limits<DestinationType>::max()) &&
           value>=static_cast<SourceType>(cpstd::numeric_limits<DestinationType>::lowest());
}

template<typename DestinationType,typename SourceType>
bool ConversionFits(const SourceType, cpstd::false_type) noexcept {
    return true;
}

/// Check conversion from an internal recurrence value to the external sample representation.
template<typename DestinationType,typename SourceType>
bool Convert(const SourceType value,DestinationType& destination) noexcept {
    static_assert(cpstd::is_arithmetic<DestinationType>::value &&
                  cpstd::is_arithmetic<SourceType>::value,
                  "DspCore signal conversions require arithmetic types");
    if(!Finite(value)) return false;
    typedef cpstd::integral_constant<bool,
        cpstd::is_integral<DestinationType>::value && cpstd::is_floating_point<SourceType>::value>
        NeedsRangeCheck;
    if(!ConversionFits<DestinationType>(value, NeedsRangeCheck{})) return false;
    const DestinationType converted=static_cast<DestinationType>(value);
    if(!Finite(converted)) return false;
    destination=converted;
    return true;
}
template<typename Real> bool Buffer(const Real* p,size_t count) noexcept {
    return (count==0 || p!=nullptr) && count<=static_cast<size_t>(-1)/sizeof(Real);
}
inline float Sin(float x) noexcept { return sinf(x); }
inline double Sin(double x) noexcept { return sin(x); }
inline float Cos(float x) noexcept { return cosf(x); }
inline double Cos(double x) noexcept { return cos(x); }
inline float Sqrt(float x) noexcept { return sqrtf(x); }
inline double Sqrt(double x) noexcept { return sqrt(x); }
inline float Pow(float base,float exponent) noexcept { return powf(base,exponent); }
inline double Pow(double base,double exponent) noexcept { return pow(base,exponent); }
inline float Exp(float x) noexcept { return expf(x); }
inline double Exp(double x) noexcept { return exp(x); }
template<typename Real> Real Pi() noexcept { return static_cast<Real>(3.1415926535897932384626433832795L); }
template<typename Real> Real Abs(Real x) noexcept { return x<Real(0) ? -x : x; }
template<typename Real> Real Alpha(Real dt,Real tau) noexcept {
    if(tau==Real(0) || tau<=dt/Real(40)) return Real(1);
    const Real ratio=dt/tau;
    if(ratio>=Real(0.125)) return Real(1)-Exp(-ratio);
    Real term=ratio,sum=ratio;
    for(unsigned n=2;n<=32;++n) {
        term *= -ratio/static_cast<Real>(n);
        const Real next=sum+term;
        if(next==sum) break;
        sum=next;
    }
    return sum;
}
}}}
/// @endcond
#endif
