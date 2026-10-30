#ifndef DSPCORE_FILTERS_BIQUAD_H
#define DSPCORE_FILTERS_BIQUAD_H

#include <Foundation_Utils.h>
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Filters/IIR.h>

#include <stdint.h>

namespace DspCore::Filters::Biquad {

/** @brief The result type of every biquad designer: directly consumable by IIR::Configure. */
template <typename CoefficientType = float>
using Coefficients = typename IIR<float, CoefficientType>::Coefficients;

/** @brief RBJ biquad designs supported by this namespace. */
enum class Type : uint8_t {
    LowPass,
    HighPass,
    BandPass,
    Notch,
    AllPass,
    Peak,
    LowShelf,
    HighShelf
};

/** @brief Named controls that a biquad design can expose. */
enum class Parameter : uint8_t {
    SampleRate,
    Cutoff,
    Quality,
    GainDecibels,
    Slope
};

/** @brief UI-independent label identity for a Biquad::Parameter. */
enum class ParameterLabel : uint8_t {
    SampleRate,
    Cutoff,
    CenterFrequency,
    ShelfFrequency,
    Quality,
    Gain,
    Slope
};

/** @brief Unit identity for a Biquad::Parameter. */
enum class Unit : uint8_t {
    None,
    Hertz,
    Decibels
};

/** @brief Compact, UI-independent metadata for one Biquad::Parameter. */
struct ParameterInfo {
    Parameter parameter;
    ParameterLabel label;
    Unit unit;
};

namespace Detail {

/** @cond INTERNAL */

/** @brief Flash-resident table descriptor for one biquad design. */
struct ParameterTable {
    const ParameterInfo* data;
    cpstd::size_t size;
};

static const ParameterInfo standard[] ROMODULAR_FLASH = {
    {Parameter::SampleRate, ParameterLabel::SampleRate, Unit::Hertz},
    {Parameter::Cutoff, ParameterLabel::Cutoff, Unit::Hertz},
    {Parameter::Quality, ParameterLabel::Quality, Unit::None}
};
static const ParameterInfo peak[] ROMODULAR_FLASH = {
    {Parameter::SampleRate, ParameterLabel::SampleRate, Unit::Hertz},
    {Parameter::Cutoff, ParameterLabel::CenterFrequency, Unit::Hertz},
    {Parameter::Quality, ParameterLabel::Quality, Unit::None},
    {Parameter::GainDecibels, ParameterLabel::Gain, Unit::Decibels}
};
static const ParameterInfo shelf[] ROMODULAR_FLASH = {
    {Parameter::SampleRate, ParameterLabel::SampleRate, Unit::Hertz},
    {Parameter::Cutoff, ParameterLabel::ShelfFrequency, Unit::Hertz},
    {Parameter::Slope, ParameterLabel::Slope, Unit::None},
    {Parameter::GainDecibels, ParameterLabel::Gain, Unit::Decibels}
};

static const ParameterTable parameterTables[] ROMODULAR_FLASH = {
    {standard, 3U},
    {standard, 3U},
    {standard, 3U},
    {standard, 3U},
    {standard, 3U},
    {peak, 4U},
    {shelf, 4U},
    {shelf, 4U}
};

inline bool ReadParameterTable(const Type type, ParameterTable& result) noexcept {
    switch (type) {
    case Type::LowPass:
    case Type::HighPass:
    case Type::BandPass:
    case Type::Notch:
    case Type::AllPass:
    case Type::Peak:
    case Type::LowShelf:
    case Type::HighShelf:
        result = Foundation::Utils::Flash::Read(&parameterTables[static_cast<cpstd::size_t>(type)]);
        return true;
    }
    return false;
}

/** @endcond */

} // namespace Detail

/**
 * @brief Returns the controls required by @p type in their intended UI order.
 *
 * Ranges that depend on other controls stay in the designer validation: for
 * example, cutoff must be below half of the selected sample rate.
 */
inline cpstd::size_t ParameterCount(const Type type) noexcept {
    Detail::ParameterTable table;
    return Detail::ReadParameterTable(type, table) ? table.size : 0U;
}

/** @brief Copies one required control from program memory into @p result. */
inline bool ParameterAt(const Type type, const cpstd::size_t index, ParameterInfo& result) noexcept {
    Detail::ParameterTable table;
    if (!Detail::ReadParameterTable(type, table) || index >= table.size) {
        return false;
    }
    result = Foundation::Utils::Flash::Read(&table.data[index]);
    return true;
}

/** @brief Returns whether @p type exposes @p parameter. */
inline bool HasParameter(const Type type, const Parameter parameter) noexcept {
    const auto count = ParameterCount(type);
    for (cpstd::size_t index = 0U; index < count; ++index) {
        ParameterInfo info;
        if (ParameterAt(type, index, info) && info.parameter == parameter) {
            return true;
        }
    }
    return false;
}

namespace Detail {

template <typename CoefficientType>
inline Coefficients<CoefficientType> Make(const CoefficientType b0, const CoefficientType b1,
    const CoefficientType b2, const CoefficientType a0, const CoefficientType a1,
    const CoefficientType a2) noexcept {
    Coefficients<CoefficientType> result;
    const CoefficientType numerator[] = {b0, b1, b2};
    const CoefficientType denominator[] = {a0, a1, a2};
    result.Assign(numerator, 3U, denominator, 3U);
    return result;
}

template <typename CoefficientType>
inline bool ValidFrequency(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    static_assert(cpstd::is_same<CoefficientType, float>::value || cpstd::is_same<CoefficientType, double>::value,
        "Biquad coefficient type must be float or double");
    return DspCore::Detail::Signal::Finite(sampleRate) && DspCore::Detail::Signal::Finite(cutoff) &&
        DspCore::Detail::Signal::Finite(quality) &&
        sampleRate > CoefficientType{} && cutoff > CoefficientType{} &&
        cutoff < sampleRate / CoefficientType{2} && quality > CoefficientType{};
}

template <typename CoefficientType>
inline void Terms(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality, CoefficientType& cosine, CoefficientType& alpha) noexcept {
    const auto omega = CoefficientType{2} * DspCore::Detail::Signal::Pi<CoefficientType>() * cutoff / sampleRate;
    cosine = DspCore::Detail::Signal::Cos(omega);
    alpha = DspCore::Detail::Signal::Sin(omega) / (CoefficientType{2} * quality);
}

template <typename CoefficientType>
inline Coefficients<CoefficientType> Invalid() noexcept { return {}; }

} // namespace Detail

/** @brief RBJ low-pass coefficients. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> LowPass(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality)) return Detail::Invalid<CoefficientType>();
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    return Detail::Make((CoefficientType{1} - cosine) / CoefficientType{2}, CoefficientType{1} - cosine,
        (CoefficientType{1} - cosine) / CoefficientType{2}, CoefficientType{1} + alpha,
        -CoefficientType{2} * cosine, CoefficientType{1} - alpha);
}

/** @brief RBJ high-pass coefficients. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> HighPass(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality)) return Detail::Invalid<CoefficientType>();
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    return Detail::Make((CoefficientType{1} + cosine) / CoefficientType{2}, -(CoefficientType{1} + cosine),
        (CoefficientType{1} + cosine) / CoefficientType{2}, CoefficientType{1} + alpha,
        -CoefficientType{2} * cosine, CoefficientType{1} - alpha);
}

/** @brief RBJ constant-0-dB-peak band-pass coefficients. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> BandPass(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality)) return Detail::Invalid<CoefficientType>();
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    return Detail::Make(alpha, CoefficientType{}, -alpha, CoefficientType{1} + alpha,
        -CoefficientType{2} * cosine, CoefficientType{1} - alpha);
}

/** @brief RBJ notch coefficients. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> Notch(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality)) return Detail::Invalid<CoefficientType>();
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    return Detail::Make(CoefficientType{1}, -CoefficientType{2} * cosine, CoefficientType{1},
        CoefficientType{1} + alpha, -CoefficientType{2} * cosine, CoefficientType{1} - alpha);
}

/** @brief RBJ all-pass coefficients. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> AllPass(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality)) return Detail::Invalid<CoefficientType>();
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    return Detail::Make(CoefficientType{1} - alpha, -CoefficientType{2} * cosine,
        CoefficientType{1} + alpha, CoefficientType{1} + alpha, -CoefficientType{2} * cosine,
        CoefficientType{1} - alpha);
}

/** @brief RBJ peaking-EQ coefficients with gain in decibels. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> Peak(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType quality, const CoefficientType gainDecibels) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, quality) || !DspCore::Detail::Signal::Finite(gainDecibels)) {
        return Detail::Invalid<CoefficientType>();
    }
    CoefficientType cosine{}, alpha{};
    Detail::Terms(sampleRate, cutoff, quality, cosine, alpha);
    const auto amplitude = DspCore::Detail::Signal::Pow(CoefficientType{10}, gainDecibels / CoefficientType{40});
    return Detail::Make(CoefficientType{1} + alpha * amplitude, -CoefficientType{2} * cosine,
        CoefficientType{1} - alpha * amplitude, CoefficientType{1} + alpha / amplitude,
        -CoefficientType{2} * cosine, CoefficientType{1} - alpha / amplitude);
}

/** @brief RBJ low-shelf coefficients; @p slope is in the closed interval (0, 1]. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> LowShelf(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType slope, const CoefficientType gainDecibels) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, slope) || slope > CoefficientType{1} ||
        !DspCore::Detail::Signal::Finite(gainDecibels)) {
        return Detail::Invalid<CoefficientType>();
    }
    const auto omega = CoefficientType{2} * DspCore::Detail::Signal::Pi<CoefficientType>() * cutoff / sampleRate;
    const auto cosine = DspCore::Detail::Signal::Cos(omega);
    const auto sine = DspCore::Detail::Signal::Sin(omega);
    const auto amplitude = DspCore::Detail::Signal::Pow(CoefficientType{10}, gainDecibels / CoefficientType{40});
    const auto alpha = sine / CoefficientType{2} * DspCore::Detail::Signal::Sqrt(
        (amplitude + CoefficientType{1} / amplitude) *
        (CoefficientType{1} / slope - CoefficientType{1}) + CoefficientType{2});
    const auto beta = CoefficientType{2} * DspCore::Detail::Signal::Sqrt(amplitude) * alpha;
    const auto plus = amplitude + CoefficientType{1};
    const auto minus = amplitude - CoefficientType{1};
    return Detail::Make(amplitude * (plus - minus * cosine + beta),
        CoefficientType{2} * amplitude * (minus - plus * cosine),
        amplitude * (plus - minus * cosine - beta), plus + minus * cosine + beta,
        -CoefficientType{2} * (minus + plus * cosine), plus + minus * cosine - beta);
}

/** @brief RBJ high-shelf coefficients; @p slope is in the closed interval (0, 1]. */
template <typename CoefficientType>
inline Coefficients<CoefficientType> HighShelf(const CoefficientType sampleRate, const CoefficientType cutoff,
    const CoefficientType slope, const CoefficientType gainDecibels) noexcept {
    if (!Detail::ValidFrequency(sampleRate, cutoff, slope) || slope > CoefficientType{1} ||
        !DspCore::Detail::Signal::Finite(gainDecibels)) {
        return Detail::Invalid<CoefficientType>();
    }
    const auto omega = CoefficientType{2} * DspCore::Detail::Signal::Pi<CoefficientType>() * cutoff / sampleRate;
    const auto cosine = DspCore::Detail::Signal::Cos(omega);
    const auto sine = DspCore::Detail::Signal::Sin(omega);
    const auto amplitude = DspCore::Detail::Signal::Pow(CoefficientType{10}, gainDecibels / CoefficientType{40});
    const auto alpha = sine / CoefficientType{2} * DspCore::Detail::Signal::Sqrt(
        (amplitude + CoefficientType{1} / amplitude) *
        (CoefficientType{1} / slope - CoefficientType{1}) + CoefficientType{2});
    const auto beta = CoefficientType{2} * DspCore::Detail::Signal::Sqrt(amplitude) * alpha;
    const auto plus = amplitude + CoefficientType{1};
    const auto minus = amplitude - CoefficientType{1};
    return Detail::Make(amplitude * (plus + minus * cosine + beta),
        -CoefficientType{2} * amplitude * (minus + plus * cosine),
        amplitude * (plus + minus * cosine - beta), plus - minus * cosine + beta,
        CoefficientType{2} * (minus - plus * cosine), plus - minus * cosine - beta);
}

/**
 * @brief Explicit-precision biquad coefficient designer.
 *
 * Biquad design is stateless: it has no stream SampleType or recurrence
 * StateType. CoefficientType alone controls all design arithmetic and defaults
 * to float. The namespace-level functions remain a deduction-friendly shortcut.
 */
template <typename CoefficientType = float>
struct Design final {
    static Coefficients<CoefficientType> LowPass(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality) noexcept {
        return Biquad::LowPass(sampleRate, cutoff, quality);
    }

    static Coefficients<CoefficientType> HighPass(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality) noexcept {
        return Biquad::HighPass(sampleRate, cutoff, quality);
    }

    static Coefficients<CoefficientType> BandPass(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality) noexcept {
        return Biquad::BandPass(sampleRate, cutoff, quality);
    }

    static Coefficients<CoefficientType> Notch(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality) noexcept {
        return Biquad::Notch(sampleRate, cutoff, quality);
    }

    static Coefficients<CoefficientType> AllPass(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality) noexcept {
        return Biquad::AllPass(sampleRate, cutoff, quality);
    }

    static Coefficients<CoefficientType> Peak(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType quality,
        const CoefficientType gainDecibels) noexcept {
        return Biquad::Peak(sampleRate, cutoff, quality, gainDecibels);
    }

    static Coefficients<CoefficientType> LowShelf(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType slope,
        const CoefficientType gainDecibels) noexcept {
        return Biquad::LowShelf(sampleRate, cutoff, slope, gainDecibels);
    }

    static Coefficients<CoefficientType> HighShelf(const CoefficientType sampleRate,
        const CoefficientType cutoff, const CoefficientType slope,
        const CoefficientType gainDecibels) noexcept {
        return Biquad::HighShelf(sampleRate, cutoff, slope, gainDecibels);
    }
};

/** @brief Returns true when a three-coefficient denominator has poles strictly inside the unit circle. */
template <typename CoefficientSet>
inline bool IsStable(const CoefficientSet& coefficients) noexcept {
    if (coefficients.denominator.size() != 3U) return false;
    const auto a0 = coefficients.denominator[0];
    if (a0 == decltype(a0){}) return false;
    const auto a1 = coefficients.denominator[1] / a0;
    const auto a2 = coefficients.denominator[2] / a0;
    const auto one = decltype(a0){1};
    return DspCore::Detail::Signal::Finite(a1) && DspCore::Detail::Signal::Finite(a2) && a2 > -one && a2 < one &&
        a1 > -(one + a2) && a1 < one + a2;
}

} // namespace DspCore::Filters::Biquad

#endif // DSPCORE_FILTERS_BIQUAD_H
