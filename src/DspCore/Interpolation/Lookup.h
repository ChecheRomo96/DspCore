#ifndef DSPCORE_INTERPOLATION_LOOKUP_H
#define DSPCORE_INTERPOLATION_LOOKUP_H
#include <DspCore/Interpolation/Arithmetic.h>
#include <DspCore/LUT.h>
#include <math.h>

namespace DspCore { namespace Interpolation {
/// @ingroup DspCore_Interpolation
/// Clamp/Reject use a closed [0,1] domain; Wrap treats all N samples as a cycle.
enum class Boundary : unsigned char { Clamp, Wrap, Reject };
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename Real, typename T>
Real Sample(T value) noexcept {
    const long double wide = static_cast<long double>(value);
    const long double limit = static_cast<long double>(cpstd::numeric_limits<Real>::max());
    if (wide != wide) return cpstd::numeric_limits<Real>::quiet_NaN();
    if (wide > limit) return cpstd::numeric_limits<Real>::infinity();
    if (wide < -limit) return -cpstd::numeric_limits<Real>::infinity();
    return static_cast<Real>(wide);
}
template<typename Real>
bool Axis(size_t count, Real coordinate, Boundary boundary,
          size_t& first, size_t& second, Real& fraction) noexcept {
    if (count == 0 || !Finite(coordinate)) return false;
    if (boundary != Boundary::Clamp && boundary != Boundary::Wrap && boundary != Boundary::Reject)
        return false;
    if (boundary == Boundary::Reject && (coordinate < Real(0) || coordinate > Real(1))) return false;
    if (count == 1) { first = second = 0; fraction = Real(0); return true; }
    Real normalized = coordinate;
    const bool wrap = boundary == Boundary::Wrap;
    if (wrap) {
        normalized = static_cast<Real>(fmod(static_cast<double>(coordinate), 1.0));
        if (normalized < Real(0)) normalized += Real(1);
        // Negative sub-ulp offsets can round to one after addition.
        if (normalized >= Real(1)) normalized = Real(0);
    } else {
        if (normalized <= Real(0)) { first = second = 0; fraction = Real(0); return true; }
        if (normalized >= Real(1)) { first = second = count - 1; fraction = Real(0); return true; }
    }
    // Require the axis extent to fit the calculation type's exact-integer range.
    // This also prevents rounding an index beyond size_t before its cast.
    const Real exactLimit = static_cast<Real>(ldexp(1.0, cpstd::numeric_limits<Real>::digits));
    const Real extent = static_cast<Real>(count);
    if (extent > exactLimit || extent >= static_cast<Real>(static_cast<size_t>(-1))) return false;
    if (static_cast<size_t>(extent) != count) return false;
    const Real scale = static_cast<Real>(wrap ? count : count - 1);
    const Real position = normalized * scale;
    if (position >= scale) {
        first = count - 1;
        second = wrap ? 0 : first;
        fraction = wrap ? Real(1) : Real(0);
        return true;
    }
    first = static_cast<size_t>(position);
    second = first + 1;
    if (second >= count) second = wrap ? 0 : first;
    fraction = position - static_cast<Real>(first);
    return true;
}
}
/// @endcond
/// @ingroup DspCore_Interpolation
/// Interpolate normalized coordinates with an explicit boundary policy; no allocation.
/// Failure (empty table, invalid coordinate/policy/extent) leaves output unchanged.
template<typename Real = float, typename T>
bool Lookup(const LUT::LUT1D<T>& table, Real x, Real& output, Boundary boundary) noexcept {
    size_t a = 0, b = 0;
    Real t = Real(0);
    if (!Detail::Axis(table.Width(), x, boundary, a, b, t)) return false;
    output = NormalizedLinear(Detail::Sample<Real>(table.GetValue(a)), Detail::Sample<Real>(table.GetValue(b)), t);
    return true;
}
/// @ingroup DspCore_Interpolation
/// Bilinear normalized lookup; the boundary policy applies independently to each axis.
template<typename Real = float, typename T>
bool Lookup(const LUT::LUT2D<T>& table, Real x, Real y, Real& output, Boundary boundary) noexcept {
    size_t a = 0, b = 0, c = 0, d = 0;
    Real tx = Real(0), ty = Real(0);
    if (!Detail::Axis(table.Width(), x, boundary, a, b, tx) ||
        !Detail::Axis(table.Height(), y, boundary, c, d, ty)) return false;
    output = NormalizedBilinear(Detail::Sample<Real>(table.GetValue(a,c)), Detail::Sample<Real>(table.GetValue(b,c)),
                      Detail::Sample<Real>(table.GetValue(a,d)), Detail::Sample<Real>(table.GetValue(b,d)), tx, ty);
    return true;
}
/// @ingroup DspCore_Interpolation
/// Trilinear normalized lookup; singleton axes are constant without division.
template<typename Real = float, typename T>
bool Lookup(const LUT::LUT3D<T>& table, Real x, Real y, Real z, Real& output, Boundary boundary) noexcept {
    size_t a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
    Real tx = Real(0), ty = Real(0), tz = Real(0);
    if (!Detail::Axis(table.Width(), x, boundary, a, b, tx) ||
        !Detail::Axis(table.Height(), y, boundary, c, d, ty) ||
        !Detail::Axis(table.Depth(), z, boundary, e, f, tz)) return false;
    output = NormalizedTrilinear(Detail::Sample<Real>(table.GetValue(a,c,e)), Detail::Sample<Real>(table.GetValue(b,c,e)),
                       Detail::Sample<Real>(table.GetValue(a,d,e)), Detail::Sample<Real>(table.GetValue(b,d,e)),
                       Detail::Sample<Real>(table.GetValue(a,c,f)), Detail::Sample<Real>(table.GetValue(b,c,f)),
                       Detail::Sample<Real>(table.GetValue(a,d,f)), Detail::Sample<Real>(table.GetValue(b,d,f)), tx, ty, tz);
    return true;
}
}}
#endif
