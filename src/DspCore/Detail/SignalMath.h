#ifndef DSPCORE_DETAIL_SIGNAL_MATH_H
#define DSPCORE_DETAIL_SIGNAL_MATH_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>
#include <stddef.h>
#include <math.h>
/// @cond DSPCORE_INTERNAL
namespace DspCore { namespace Detail { namespace Signal {
template<typename Real> bool Finite(Real x) noexcept {
    static_assert(cpstd::is_same<Real,float>::value || cpstd::is_same<Real,double>::value,
                  "Signal processing requires float or double");
    return x<=cpstd::numeric_limits<Real>::max() && x>=-cpstd::numeric_limits<Real>::max();
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
