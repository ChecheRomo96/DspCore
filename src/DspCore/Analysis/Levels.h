#ifndef DSPCORE_ANALYSIS_LEVELS_H
#define DSPCORE_ANALYSIS_LEVELS_H
#include <DspCore/Detail/SignalMath.h>
namespace DspCore { namespace Analysis {
/// @ingroup DspCore_Analysis
/// Absolute block peak; empty input produces zero. Failure preserves output.
template<typename Real> bool Peak(const Real* input,size_t count,Real& output) noexcept {
    if(!DspCore::Detail::Signal::Buffer(input,count)) return false;
    Real maximum=Real(0);
    for(size_t i=0;i<count;++i) {
        if(!DspCore::Detail::Signal::Finite(input[i])) return false;
        const Real magnitude=DspCore::Detail::Signal::Abs(input[i]);
        if(magnitude>maximum) maximum=magnitude;
    }
    output=maximum;return true;
}
/// @ingroup DspCore_Analysis
/// Block RMS without mean removal; scaled arithmetic avoids squaring large samples.
template<typename Real> bool RMS(const Real* input,size_t count,Real& output) noexcept {
    Real scale=Real(0);
    if(!Peak(input,count,scale)) return false;
    if(scale==Real(0)) { output=Real(0);return true; }
    Real sum=Real(0);
    for(size_t i=0;i<count;++i) {
        const Real normalized=input[i]/scale;
        sum+=normalized*normalized;
    }
    const Real mean=sum/static_cast<Real>(count);
    const Real bounded=mean>Real(1) ? Real(1) : mean;
    output=scale*DspCore::Detail::Signal::Sqrt(bounded);return true;
}
}}
#endif
