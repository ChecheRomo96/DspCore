#ifndef DSPCORE_INTERPOLATION_ARITHMETIC_H
#define DSPCORE_INTERPOLATION_ARITHMETIC_H
#include <DspCore_BuildSettings.h>
#include <CPtype_traits.h>
#include <CPlimits.h>

namespace DspCore { namespace Interpolation {
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename Real>
bool Finite(Real value) noexcept {
    static_assert(cpstd::is_same<Real, float>::value || cpstd::is_same<Real, double>::value,
                  "Interpolation arithmetic must explicitly use float or double");
    return value <= cpstd::numeric_limits<Real>::max() &&
           value >= -cpstd::numeric_limits<Real>::max();
}

template<typename Real>
Real NormalizedCoordinate(Real coordinate0, Real coordinate1, Real coordinate) noexcept {
    static_assert(cpstd::is_same<Real, float>::value || cpstd::is_same<Real, double>::value,
                  "Interpolation arithmetic must explicitly use float or double");
    return coordinate0 == coordinate1 ? Real(0) : (coordinate - coordinate0) / (coordinate1 - coordinate0);
}
}
/// @endcond
/// @ingroup DspCore_Interpolation
/// Interpolates values with an already normalized coordinate; t=0/1 selects the exact endpoint.
template<typename Real>
Real NormalizedLinear(Real value0, Real value1, Real t) noexcept {
    static_assert(cpstd::is_same<Real, float>::value || cpstd::is_same<Real, double>::value,
                  "Interpolation arithmetic must explicitly use float or double");
    if (t == Real(0)) return value0;
    if (t == Real(1)) return value1;
    // Weighted form avoids overflowing value1-value0 for opposite-sign finite endpoints.
    if ((value0 <= Real(0) && value1 >= Real(0)) || (value0 >= Real(0) && value1 <= Real(0)))
        return (Real(1) - t) * value0 + t * value1;
    return value0 + t * (value1 - value0);
}
/// @ingroup DspCore_Interpolation
/// Interpolates x-fast corners with already normalized x/y coordinates.
template<typename Real>
Real NormalizedBilinear(Real f00, Real f10, Real f01, Real f11, Real tx, Real ty) noexcept {
    return NormalizedLinear(NormalizedLinear(f00, f10, tx), NormalizedLinear(f01, f11, tx), ty);
}
/// @ingroup DspCore_Interpolation
/// Interpolates x-fast corners with already normalized x/y/z coordinates.
template<typename Real>
Real NormalizedTrilinear(Real f000, Real f100, Real f010, Real f110,
    Real f001, Real f101, Real f011, Real f111, Real tx, Real ty, Real tz) noexcept {
    return NormalizedLinear(NormalizedBilinear(f000, f100, f010, f110, tx, ty),
        NormalizedBilinear(f001, f101, f011, f111, tx, ty), tz);
}
/// @ingroup DspCore_Interpolation
/// Interpolates two values at a physical coordinate; a collapsed axis selects value0.
template<typename Real>
Real LinearAt(Real x0, Real x1, Real value0, Real value1, Real x) noexcept {
    return NormalizedLinear(value0, value1, Detail::NormalizedCoordinate(x0, x1, x));
}
/// @ingroup DspCore_Interpolation
/// Interpolates x-fast corners at physical x/y coordinates; each collapsed axis selects its 0-side.
template<typename Real>
Real BilinearAt(Real x0, Real x1, Real y0, Real y1,
    Real f00, Real f10, Real f01, Real f11, Real x, Real y) noexcept {
    return NormalizedBilinear(f00, f10, f01, f11,
        Detail::NormalizedCoordinate(x0, x1, x), Detail::NormalizedCoordinate(y0, y1, y));
}
/// @ingroup DspCore_Interpolation
/// Interpolates x-fast corners at physical x/y/z coordinates; each collapsed axis selects its 0-side.
template<typename Real>
Real TrilinearAt(Real x0, Real x1, Real y0, Real y1, Real z0, Real z1,
    Real f000, Real f100, Real f010, Real f110, Real f001, Real f101, Real f011, Real f111,
    Real x, Real y, Real z) noexcept {
    return NormalizedTrilinear(f000, f100, f010, f110, f001, f101, f011, f111,
        Detail::NormalizedCoordinate(x0, x1, x), Detail::NormalizedCoordinate(y0, y1, y),
        Detail::NormalizedCoordinate(z0, z1, z));
}
}} // namespace DspCore::Interpolation
#endif
