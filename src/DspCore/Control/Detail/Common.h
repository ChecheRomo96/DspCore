#ifndef DSPCORE_CONTROL_DETAIL_COMMON_H
#define DSPCORE_CONTROL_DETAIL_COMMON_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>
#include <math.h>
/// @cond DSPCORE_INTERNAL
namespace DspCore { namespace Control { namespace Detail {
template<typename Real> bool Finite(Real x) noexcept {
    static_assert(cpstd::is_same<Real,float>::value || cpstd::is_same<Real,double>::value,
                  "Control requires float or double arithmetic");
    return x<=cpstd::numeric_limits<Real>::max() && x>=-cpstd::numeric_limits<Real>::max();
}
template<typename Real> Real Clamp(Real x, Real low, Real high) noexcept {
    return x<low ? low : (x>high ? high : x);
}
template<typename Real> Real Mix(Real a, Real b, Real t) noexcept {
    if(t==Real(0)) return a;
    if(t==Real(1)) return b;
    if((a<=Real(0) && b>=Real(0)) || (a>=Real(0) && b<=Real(0))) return (Real(1)-t)*a+t*b;
    return a+t*(b-a);
}
inline float Exp(float x) noexcept { return expf(x); }
inline double Exp(double x) noexcept { return exp(x); }
template<typename Real> Real Alpha(Real dt, Real tau) noexcept {
    if(tau==Real(0) || tau<=dt/Real(40)) return Real(1);
    const Real ratio=dt/tau;
    if(ratio>=Real(0.125)) return Real(1)-Exp(-ratio);
    Real term=ratio, sum=ratio;
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
