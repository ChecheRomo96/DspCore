#ifndef DSPCORE_FILTERS_BIQUAD_H
#define DSPCORE_FILTERS_BIQUAD_H
#include <DspCore/Detail/SignalMath.h>
namespace DspCore { namespace Filters { namespace Biquad {
/// @ingroup DspCore_Filters_Biquad
/// @brief Three numerator and three denominator coefficients, including a0.
template<typename T=float> struct Coefficients {
    T numerator[3]={T(1),T(0),T(0)};
    T denominator[3]={T(1),T(0),T(0)};
};
/// @ingroup DspCore_Filters_Biquad
/// Check finite, normalizable coefficients and strict second-order pole stability.
template<typename T> bool IsStable(const Coefficients<T>& c) noexcept {
    if(!DspCore::Detail::Signal::Finite(c.denominator[0]) || c.denominator[0]==T(0)) return false;
    for(size_t i=0;i<3;++i) {
        if(!DspCore::Detail::Signal::Finite(c.numerator[i]) || !DspCore::Detail::Signal::Finite(c.denominator[i]) ||
           !DspCore::Detail::Signal::Finite(c.numerator[i]/c.denominator[0]) ||
           !DspCore::Detail::Signal::Finite(c.denominator[i]/c.denominator[0])) return false;
    }
    const T a1=c.denominator[1]/c.denominator[0],a2=c.denominator[2]/c.denominator[0];
    return a2>T(-1) && a2<T(1) && a1>-(T(1)+a2) && a1<T(1)+a2;
}
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename T> Coefficients<T> Invalid() noexcept {
    Coefficients<T> result;
    result.denominator[0]=T(0);
    return result;
}
template<typename T> Coefficients<T> Calculate(T sampleRate,T cutoff,T q,bool highpass) noexcept {
    if(!DspCore::Detail::Signal::Finite(sampleRate) || sampleRate<=T(0) ||
       !DspCore::Detail::Signal::Finite(cutoff) || cutoff<=T(0) || cutoff>=sampleRate/T(2) ||
       !DspCore::Detail::Signal::Finite(q) || q<=T(0)) return Invalid<T>();
    const T omega=T(2)*DspCore::Detail::Signal::Pi<T>()*(cutoff/sampleRate);
    const T alpha=(DspCore::Detail::Signal::Sin(omega)/T(2))/q;
    if(!DspCore::Detail::Signal::Finite(alpha) || alpha<=T(0)) return Invalid<T>();
    const T half=highpass ? DspCore::Detail::Signal::Cos(omega/T(2)) : DspCore::Detail::Signal::Sin(omega/T(2));
    Coefficients<T> c;
    c.numerator[0]=half*half;c.numerator[1]=(highpass ? T(-2) : T(2))*c.numerator[0];c.numerator[2]=c.numerator[0];
    c.denominator[0]=T(1)+alpha;c.denominator[1]=-T(2)*DspCore::Detail::Signal::Cos(omega);c.denominator[2]=T(1)-alpha;
    return IsStable(c) ? c : Invalid<T>();
}
}
/// @endcond
/// @ingroup DspCore_Filters_Biquad
/// Return low-pass coefficients by value; invalid designs have denominator[0]==0.
template<typename T> Coefficients<T> LowPass(T sampleRate,T cutoff,T q) noexcept {
    return Detail::Calculate(sampleRate,cutoff,q,false);
}
/// @ingroup DspCore_Filters_Biquad
/// Return high-pass coefficients by value; normalize a0 when configuring IIR.
template<typename T> Coefficients<T> HighPass(T sampleRate,T cutoff,T q) noexcept {
    return Detail::Calculate(sampleRate,cutoff,q,true);
}
}}}
#endif
