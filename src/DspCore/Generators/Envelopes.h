#ifndef DSPCORE_GENERATORS_ENVELOPES_H
#define DSPCORE_GENERATORS_ENVELOPES_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Envelopes {
/// @cond DSPCORE_INTERNAL
namespace Detail {
enum class Curve { Linear, Exponential, Logarithmic };
template<typename Real>
bool Segment(Real* out, Real start, Real end, Real curve, size_t total, size_t first, size_t last, Curve kind) noexcept {
    if (!Generators::Detail::Buffer(out,total) || !Generators::Detail::Finite(start) ||
        !Generators::Detail::Finite(end) || !Generators::Detail::Finite(curve) || curve<Real(0) ||
        !Generators::Detail::Range(total,first,last)) return false;
    const size_t length=last-first;
    const bool linear=kind==Curve::Linear || curve <= Real(8)*cpstd::numeric_limits<Real>::epsilon();
    Real denominator=Real(1);
    if (!linear) denominator=kind==Curve::Exponential ? -Generators::Detail::Expm1(-curve) : Generators::Detail::Log1p(curve);
    for(size_t i=0;i<length;++i) {
        if (i==0) { out[first+i]=start; continue; }
        if (i==length-1) { out[first+i]=end; continue; }
        const Real t=static_cast<Real>(i)/static_cast<Real>(length-1);
        Real shape=t;
        if (!linear) shape=kind==Curve::Exponential ? -Generators::Detail::Expm1(-curve*t)/denominator
                                                    : Generators::Detail::Log1p(curve*t)/denominator;
        out[first+i]=Generators::Detail::Mix(start,end,Generators::Detail::Unit(shape));
    }
    return true;
}
}
/// @endcond
/// @ingroup DspCore_Generators_Envelopes
/// Fill half-open [first,last); empty ranges are no-ops, singleton ranges write start.
template<typename Real>
bool LinearSegment(Real* out, Real start, Real end, size_t total,
                   size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,Real(0),total,first,last,Detail::Curve::Linear);
}
/// @ingroup DspCore_Generators_Envelopes
/// Normalized (1-exp(-curve*t))/(1-exp(-curve)); curve=0 has a linear limit.
template<typename Real>
bool ExpSegment(Real* out, Real start, Real end, Real curve, size_t total,
                size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,curve,total,first,last,Detail::Curve::Exponential);
}
/// @ingroup DspCore_Generators_Envelopes
/// Normalized log(1+curve*t)/log(1+curve); curve=0 has a linear limit.
template<typename Real>
bool LogSegment(Real* out, Real start, Real end, Real curve, size_t total,
                size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,curve,total,first,last,Detail::Curve::Logarithmic);
}
}}}
#endif
