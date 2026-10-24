#ifndef DSPCORE_DETAIL_SIGNAL_MATH_H
#define DSPCORE_DETAIL_SIGNAL_MATH_H

#include <CPlimits.h>
#include <CPtype_traits.h>
#include <math.h>

namespace DspCore::Detail::Signal {

template <typename SampleType>
inline bool Finite(const SampleType value) noexcept {
    static_assert(cpstd::is_arithmetic<SampleType>::value,
        "DspCore signal values must be arithmetic");
    if constexpr (cpstd::is_integral<SampleType>::value) {
        return true;
    }
    return value <= cpstd::numeric_limits<SampleType>::max() &&
        value >= -cpstd::numeric_limits<SampleType>::max();
}

/** @brief Checked conversion at the boundary between internal filter state and a signal stream. */
template <typename DestinationType, typename SourceType>
inline bool Convert(const SourceType value, DestinationType& destination) noexcept {
    static_assert(cpstd::is_arithmetic<DestinationType>::value && cpstd::is_arithmetic<SourceType>::value,
        "DspCore signal conversions require arithmetic types");
    if (!Finite(value)) {
        return false;
    }

    if constexpr (cpstd::is_integral<DestinationType>::value && cpstd::is_floating_point<SourceType>::value) {
        if (value > static_cast<SourceType>(cpstd::numeric_limits<DestinationType>::max()) ||
            value < static_cast<SourceType>(cpstd::numeric_limits<DestinationType>::lowest())) {
            return false;
        }
    }

    const DestinationType converted = static_cast<DestinationType>(value);
    if (!Finite(converted)) {
        return false;
    }
    destination = converted;
    return true;
}

inline float Sin(const float value) noexcept { return sinf(value); }
inline double Sin(const double value) noexcept { return sin(value); }
inline float Cos(const float value) noexcept { return cosf(value); }
inline double Cos(const double value) noexcept { return cos(value); }
inline float Sqrt(const float value) noexcept { return sqrtf(value); }
inline double Sqrt(const double value) noexcept { return sqrt(value); }
inline float Pow(const float base, const float exponent) noexcept { return powf(base, exponent); }
inline double Pow(const double base, const double exponent) noexcept { return pow(base, exponent); }

template <typename SampleType>
constexpr SampleType Pi() noexcept {
    return static_cast<SampleType>(3.1415926535897932384626433832795L);
}

} // namespace DspCore::Detail::Signal

#endif // DSPCORE_DETAIL_SIGNAL_MATH_H
