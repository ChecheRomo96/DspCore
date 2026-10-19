#ifndef DSPCORE_GENERATORS_SHAPING_H
#define DSPCORE_GENERATORS_SHAPING_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Shaping {
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename Real>
bool Sigmoid(Real* buffer, size_t count, Real drive, Real scale, bool atanCurve, bool half, bool normalized) noexcept {
    if (!Generators::Detail::Buffer(buffer,count) || !Generators::Detail::Finite(drive) || drive<=Real(0) ||
        !Generators::Detail::Finite(scale) || scale<Real(0)) return false;
    const Real denominator = atanCurve ? Generators::Detail::Atan(drive) : Generators::Detail::Tanh(drive);
    for(size_t i=0;i<count;++i) {
        const Real t=count>1 ? static_cast<Real>(i)/static_cast<Real>(count-1) : (half ? Real(0) : Real(0.5));
        const Real x=half ? t : Real(2)*t-Real(1);
        Real y=atanCurve ? Generators::Detail::Atan(drive*x) : Generators::Detail::Tanh(drive*x);
        if (normalized) {
            y=drive <= Real(8)*cpstd::numeric_limits<Real>::epsilon() ? x : y/denominator;
            const Real magnitude=Generators::Detail::Unit(Generators::Detail::Abs(y));
            y=x<Real(0) ? -magnitude : magnitude;
            if (count>1 && i==0) y=half ? Real(0) : Real(-1);
            if (count>1 && i==count-1) y=Real(1);
        }
        // Raw atan is bounded by pi/2 rather than one; validate its scale below.
        buffer[i]=y*scale;
    }
    return true;
}
}
/// @endcond
/// @ingroup DspCore_Generators_Shaping
/// Atan curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool Atan(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {
    if (!Generators::Detail::Finite(scale) || scale > cpstd::numeric_limits<Real>::max() / (Generators::Detail::Pi<Real>()/Real(2))) return false;
    return Detail::Sigmoid(out,count,drive,scale,true,false,false);
}
/// @ingroup DspCore_Generators_Shaping
/// AtanNorm curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool AtanNorm(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,true,false,true);
}
/// @ingroup DspCore_Generators_Shaping
/// AtanHalf curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool AtanHalf(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {
    if (!Generators::Detail::Finite(scale) || scale > cpstd::numeric_limits<Real>::max() / (Generators::Detail::Pi<Real>()/Real(2))) return false;
    return Detail::Sigmoid(out,count,drive,scale,true,true,false);
}
/// @ingroup DspCore_Generators_Shaping
/// AtanHalfNorm curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool AtanHalfNorm(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,true,true,true);
}
/// @ingroup DspCore_Generators_Shaping
/// Tanh curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool Tanh(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,false,false,false);
}
/// @ingroup DspCore_Generators_Shaping
/// TanhNorm curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool TanhNorm(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,false,false,true);
}
/// @ingroup DspCore_Generators_Shaping
/// TanhHalf curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool TanhHalf(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,false,true,false);
}
/// @ingroup DspCore_Generators_Shaping
/// TanhHalfNorm curve; positive finite drive and nonnegative finite scale are required.
template<typename Real> bool TanhHalfNorm(Real* out, size_t count, Real drive, Real scale=Real(1)) noexcept {

    return Detail::Sigmoid(out,count,drive,scale,false,true,true);
}
/// @ingroup DspCore_Generators_Shaping
/// Unipolar x^gamma on [0,1], with exact endpoints; singleton samples x=0.
template<typename Real> bool Gamma(Real* out, size_t count, Real gamma, Real scale=Real(1)) noexcept {
    if (!Generators::Detail::Buffer(out,count) || !Generators::Detail::Finite(gamma) || gamma<=Real(0) ||
        !Generators::Detail::Finite(scale) || scale<Real(0)) return false;
    for(size_t i=0;i<count;++i) {
        const Real x=count>1 ? static_cast<Real>(i)/static_cast<Real>(count-1) : Real(0);
        const Real y=i==0 ? Real(0) : (i==count-1 ? Real(1) : Generators::Detail::Unit(Generators::Detail::Pow(x,gamma)));
        out[i]=y*scale;
    }
    return true;
}
/// @ingroup DspCore_Generators_Shaping
/// Gamma with exponent pow(gain,morph); reject underflow/overflow of that exponent before writing.
template<typename Real> bool GammaMorph(Real* out, size_t count, Real morph, Real gain, Real scale=Real(1)) noexcept {
    if (!Generators::Detail::Finite(morph) || !Generators::Detail::Finite(gain) || gain<=Real(0)) return false;
    const Real exponent=Generators::Detail::Pow(gain,morph);
    return Gamma(out,count,exponent,scale);
}
}}}
#endif
