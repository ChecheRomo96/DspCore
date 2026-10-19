#ifndef DSPCORE_GENERATORS_DETAIL_COMMON_H
#define DSPCORE_GENERATORS_DETAIL_COMMON_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>
#include <stddef.h>
#include <stdint.h>
#include <math.h>
/// @cond DSPCORE_INTERNAL
namespace DspCore { namespace Generators { namespace Detail {
template<typename Real> bool Finite(Real x) noexcept {
    static_assert(cpstd::is_same<Real,float>::value || cpstd::is_same<Real,double>::value,
                  "Generators require float or double output and arithmetic");
    return x <= cpstd::numeric_limits<Real>::max() && x >= -cpstd::numeric_limits<Real>::max();
}
template<typename Real> bool Buffer(Real* data, size_t count) noexcept {
    return (count == 0 || data != nullptr) && count <= static_cast<size_t>(-1) / sizeof(Real);
}
template<typename Real> bool Gain(Real amplitude, Real offset) noexcept {
    return Finite(amplitude) && amplitude >= Real(0) && Finite(offset) &&
           Finite(offset + amplitude) && Finite(offset - amplitude);
}
#define DSPCORE_GENERATOR_MATH(Name, F, D) \
    inline float Name(float x) noexcept { return F(x); } \
    inline double Name(double x) noexcept { return D(x); }
DSPCORE_GENERATOR_MATH(Sin, sinf, sin)
DSPCORE_GENERATOR_MATH(Cos, cosf, cos)
DSPCORE_GENERATOR_MATH(Exp, expf, exp)
DSPCORE_GENERATOR_MATH(Log, logf, log)
DSPCORE_GENERATOR_MATH(Atan, atanf, atan)
DSPCORE_GENERATOR_MATH(Tanh, tanhf, tanh)
DSPCORE_GENERATOR_MATH(Abs, fabsf, fabs)
#undef DSPCORE_GENERATOR_MATH
inline float Pow(float x, float y) noexcept { return powf(x,y); }
inline double Pow(double x, double y) noexcept { return pow(x,y); }
inline float Mod(float x) noexcept { return fmodf(x,1.f); }
inline double Mod(double x) noexcept { return fmod(x,1.); }
template<typename Real> Real Pi() noexcept { return static_cast<Real>(3.1415926535897932384626433832795L); }
template<typename Real> Real Wrap(Real x) noexcept {
    Real phase = Mod(x);
    if (phase < Real(0)) phase += Real(1);
    return phase >= Real(1) ? Real(0) : phase;
}
template<typename Real> Real Unit(Real value) noexcept {
    return value < Real(0) ? Real(0) : (value > Real(1) ? Real(1) : value);
}
// Portable cancellation-safe small-argument series: AVR libc lacks expm1/log1p.
template<typename Real> Real Expm1(Real x) noexcept {
    if (Abs(x) >= Real(0.125)) return Exp(x) - Real(1);
    Real term=x, sum=x;
    for (unsigned n=2;n<=32;++n) {
        term *= x / static_cast<Real>(n);
        const Real next = sum + term;
        if (next == sum) break;
        sum = next;
    }
    return sum;
}
template<typename Real> Real Log1p(Real x) noexcept {
    if (Abs(x) >= Real(0.125)) return Log(Real(1)+x);
    Real power=x, sum=x;
    for (unsigned n=2;n<=32;++n) {
        power *= -x;
        const Real next = sum + power / static_cast<Real>(n);
        if (next == sum) break;
        sum = next;
    }
    return sum;
}
template<typename Real> Real Mix(Real a, Real b, Real t) noexcept {
    if (t == Real(0)) return a;
    if (t == Real(1)) return b;
    if ((a <= Real(0) && b >= Real(0)) || (a >= Real(0) && b <= Real(0)))
        return (Real(1)-t)*a+t*b;
    return a+t*(b-a);
}
inline bool Range(size_t total, size_t first, size_t& last) noexcept {
    if (last == static_cast<size_t>(-1)) last=total;
    return first <= last && last <= total;
}
}}}
/// @endcond
#endif
