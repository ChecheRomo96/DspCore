#ifndef DSPCORE_ANALYSIS_LEVELS_H
#define DSPCORE_ANALYSIS_LEVELS_H
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/SampleBlock.h>
namespace DspCore { namespace Analysis {
/// @ingroup DspCore_Analysis
/// Absolute peak of a read-only sample block; an empty valid block produces zero.
template<typename Real>
bool Peak(const Processing::SampleBlock<const Real> input, Real& output) noexcept {
    if (!input.IsValid()) return false;
    Real maximum=Real(0);
    for (cpstd::size_t index=0U; index<input.size; ++index) {
        if(!DspCore::Detail::Signal::Finite(input.data[index])) return false;
        const Real magnitude=DspCore::Detail::Signal::Abs(input.data[index]);
        if(magnitude>maximum) maximum=magnitude;
    }
    output=maximum;return true;
}
/// @ingroup DspCore_Analysis
/// RMS of a read-only sample block without mean removal; scaled arithmetic avoids squaring large samples.
template<typename Real>
bool RMS(const Processing::SampleBlock<const Real> input, Real& output) noexcept {
    Real scale=Real(0);
    if(!Peak(input,scale)) return false;
    if(scale==Real(0)) { output=Real(0);return true; }
    Real sum=Real(0);
    for (cpstd::size_t index=0U; index<input.size; ++index) {
        const Real normalized=input.data[index]/scale;
        sum+=normalized*normalized;
    }
    const Real mean=sum/static_cast<Real>(input.size);
    const Real bounded=mean>Real(1) ? Real(1) : mean;
    output=scale*DspCore::Detail::Signal::Sqrt(bounded);return true;
}
}}
#endif
