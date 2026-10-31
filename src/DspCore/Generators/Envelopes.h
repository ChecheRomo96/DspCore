#ifndef DSPCORE_GENERATORS_ENVELOPES_H
#define DSPCORE_GENERATORS_ENVELOPES_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Envelopes {
/// @cond DSPCORE_INTERNAL
namespace Detail {
enum class Curve : unsigned char { Linear, Exponential, Logarithmic };
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
/**
 * @ingroup DspCore_Generators_Envelopes
 * @brief Generates a linear segment in a caller-owned buffer.
 *
 * For @f$L=\mathtt{last}-\mathtt{first}>1@f$, the sample at local index
 * @f$i@f$ is
 * @f[
 * t_i=\frac{i}{L-1}, \qquad y_i=\mathtt{start}+(\mathtt{end}-\mathtt{start})t_i.
 * @f]
 * Thus the first and final samples are exactly @p start and @p end. An empty
 * half-open range is a successful no-op; a one-sample range writes @p start.
 *
 * @param out Caller-owned output storage of @p total samples.
 * @param start Value written at the beginning of a nonempty segment.
 * @param end Value written at the end of a segment with two or more samples.
 * @param total Accessible sample count in @p out.
 * @param first First output index, inclusive.
 * @param last Final output index, exclusive; defaults to @p total.
 * @return `true` if all parameters and the requested range are valid.
 */
template<typename Real>
bool LinearSegment(Real* out, Real start, Real end, size_t total,
                   size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,Real(0),total,first,last,Detail::Curve::Linear);
}
/**
 * @ingroup DspCore_Generators_Envelopes
 * @brief Generates a normalized exponential segment in a caller-owned buffer.
 *
 * For @f$L=\mathtt{last}-\mathtt{first}>1@f$, the normalized coordinate and
 * curve are
 * @f[
 * t_i=\frac{i}{L-1}, \qquad
 * q_{\mathrm{exp}}(t_i)=\frac{1-e^{-ct_i}}{1-e^{-c}}, \qquad
 * y_i=\mathtt{start}+(\mathtt{end}-\mathtt{start})q_{\mathrm{exp}}(t_i),
 * @f]
 * where @f$c=\mathtt{curve}\geq0@f$. At @f$c=0@f$ (and for sufficiently
 * small curves) this uses the linear limit @f$q(t)=t@f$. The first and final
 * samples remain exactly @p start and @p end.
 *
 * @param out Caller-owned output storage of @p total samples.
 * @param start Value written at the beginning of a nonempty segment.
 * @param end Value written at the end of a segment with two or more samples.
 * @param curve Nonnegative exponential curvature @f$c@f$.
 * @param total Accessible sample count in @p out.
 * @param first First output index, inclusive.
 * @param last Final output index, exclusive; defaults to @p total.
 * @return `true` if all parameters and the requested range are valid.
 */
template<typename Real>
bool ExpSegment(Real* out, Real start, Real end, Real curve, size_t total,
                size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,curve,total,first,last,Detail::Curve::Exponential);
}
/**
 * @ingroup DspCore_Generators_Envelopes
 * @brief Generates a normalized logarithmic segment in a caller-owned buffer.
 *
 * For @f$L=\mathtt{last}-\mathtt{first}>1@f$, the normalized coordinate and
 * curve are
 * @f[
 * t_i=\frac{i}{L-1}, \qquad
 * q_{\mathrm{log}}(t_i)=\frac{\log(1+ct_i)}{\log(1+c)}, \qquad
 * y_i=\mathtt{start}+(\mathtt{end}-\mathtt{start})q_{\mathrm{log}}(t_i),
 * @f]
 * where @f$c=\mathtt{curve}\geq0@f$. At @f$c=0@f$ (and for sufficiently
 * small curves) this uses the linear limit @f$q(t)=t@f$. The first and final
 * samples remain exactly @p start and @p end.
 *
 * @param out Caller-owned output storage of @p total samples.
 * @param start Value written at the beginning of a nonempty segment.
 * @param end Value written at the end of a segment with two or more samples.
 * @param curve Nonnegative logarithmic curvature @f$c@f$.
 * @param total Accessible sample count in @p out.
 * @param first First output index, inclusive.
 * @param last Final output index, exclusive; defaults to @p total.
 * @return `true` if all parameters and the requested range are valid.
 */
template<typename Real>
bool LogSegment(Real* out, Real start, Real end, Real curve, size_t total,
                size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    return Detail::Segment(out,start,end,curve,total,first,last,Detail::Curve::Logarithmic);
}
}}}
#endif

