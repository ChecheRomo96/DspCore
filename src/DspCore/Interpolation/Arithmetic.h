#ifndef DSPCORE_INTERPOLATION_ARITHMETIC_H
#define DSPCORE_INTERPOLATION_ARITHMETIC_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>

namespace DspCore { namespace Interpolation {
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename Real> bool Finite(Real value) noexcept {
    static_assert(cpstd::is_same<Real, float>::value || cpstd::is_same<Real, double>::value,
                  "Interpolation arithmetic must explicitly use float or double");
    return value <= cpstd::numeric_limits<Real>::max() &&
           value >= -cpstd::numeric_limits<Real>::max();
}
}
/// @endcond
/// @ingroup DspCore_Interpolation
/// Unclamped interpolation; t=0/1 returns the exact corresponding endpoint.
template<typename Real>
Real Linear(Real a, Real b, Real t) noexcept {
    static_assert(cpstd::is_same<Real, float>::value || cpstd::is_same<Real, double>::value,
                  "Interpolation arithmetic must explicitly use float or double");
    if (t == Real(0)) return a;
    if (t == Real(1)) return b;
    // Weighted form avoids overflowing b-a for opposite-sign finite endpoints.
    if ((a <= Real(0) && b >= Real(0)) || (a >= Real(0) && b <= Real(0)))
        return (Real(1) - t) * a + t * b;
    return a + t * (b - a);
}
/// @ingroup DspCore_Interpolation
/// Corners use x-fast order: f00, f10, f01, f11. Coordinates are unclamped.
template<typename Real>
Real Bilinear(Real f00, Real f10, Real f01, Real f11, Real x, Real y) noexcept {
    return Linear(Linear(f00, f10, x), Linear(f01, f11, x), y);
}
/// @ingroup DspCore_Interpolation
/// Corners use x-fast order: f000,f100,f010,f110,f001,f101,f011,f111.
template<typename Real>
Real Trilinear(Real f000, Real f100, Real f010, Real f110,
               Real f001, Real f101, Real f011, Real f111,
               Real x, Real y, Real z) noexcept {
    return Linear(Bilinear(f000, f100, f010, f110, x, y),
                  Bilinear(f001, f101, f011, f111, x, y), z);
}
/// @ingroup DspCore_Interpolation
/// Normalize an ascending finite coordinate span; failure leaves t unchanged.
/// The span and normalized result must be representable; extrapolation is allowed.
template<typename Real>
bool TryUnitCoordinate(Real x, Real low, Real high, Real& t) noexcept {
    if (!Detail::Finite(x) || !Detail::Finite(low) || !Detail::Finite(high) || !(low < high))
        return false;
    const Real span = high - low;
    if (!Detail::Finite(span)) return false;
    const Real result = (x - low) / span;
    if (!Detail::Finite(result)) return false;
    t = result;
    return true;
}
/// @ingroup DspCore_Interpolation
/// Checked physical-coordinate linear interpolation; failure leaves output unchanged.
template<typename Real>
bool TryLinear(Real low, Real high, Real a, Real b, Real x, Real& output) noexcept {
    Real t = Real(0);
    if (!TryUnitCoordinate(x, low, high, t)) return false;
    output = Linear(a, b, t);
    return true;
}
/// @ingroup DspCore_Interpolation
/// Checked physical coordinates; ascending spans are required on both axes.
template<typename Real>
bool TryBilinear(Real x0, Real x1, Real y0, Real y1,
                 Real f00, Real f10, Real f01, Real f11,
                 Real x, Real y, Real& output) noexcept {
    Real tx = Real(0), ty = Real(0);
    if (!TryUnitCoordinate(x, x0, x1, tx) || !TryUnitCoordinate(y, y0, y1, ty)) return false;
    output = Bilinear(f00, f10, f01, f11, tx, ty);
    return true;
}
/// @ingroup DspCore_Interpolation
/// Checked physical coordinates; ascending spans are required on all axes.
template<typename Real>
bool TryTrilinear(Real x0, Real x1, Real y0, Real y1, Real z0, Real z1,
                  Real f000, Real f100, Real f010, Real f110,
                  Real f001, Real f101, Real f011, Real f111,
                  Real x, Real y, Real z, Real& output) noexcept {
    Real tx = Real(0), ty = Real(0), tz = Real(0);
    if (!TryUnitCoordinate(x, x0, x1, tx) || !TryUnitCoordinate(y, y0, y1, ty) ||
        !TryUnitCoordinate(z, z0, z1, tz)) return false;
    output = Trilinear(f000, f100, f010, f110, f001, f101, f011, f111, tx, ty, tz);
    return true;
}
}} // namespace DspCore::Interpolation
#endif
