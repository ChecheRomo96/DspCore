#ifndef DSPCORE_ANALYSIS_ENVELOPE_FOLLOWER_H
#define DSPCORE_ANALYSIS_ENVELOPE_FOLLOWER_H
#include <DspCore/Detail/SignalMath.h>
namespace DspCore { namespace Analysis {
/// @ingroup DspCore_Analysis
/// @brief Rectified one-pole amplitude follower with separate attack/release seconds.
template<typename Real=float> class EnvelopeFollower {
public:
    bool Configure(Real attackSeconds,Real releaseSeconds) noexcept {
        if(!DspCore::Detail::Signal::Finite(attackSeconds) || !DspCore::Detail::Signal::Finite(releaseSeconds) ||
           attackSeconds<Real(0) || releaseSeconds<Real(0)) return false;
        attack_=attackSeconds;release_=releaseSeconds;return true;
    }
    bool Reset(Real value=Real(0)) noexcept {
        if(!DspCore::Detail::Signal::Finite(value) || value<Real(0)) return false;
        value_=value;return true;
    }
    /// Finite sample and positive finite dt are required; zero times snap instantly.
    bool Update(Real sample,Real dt,Real& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(sample) || !DspCore::Detail::Signal::Finite(dt) || dt<=Real(0)) return false;
        const Real magnitude=DspCore::Detail::Signal::Abs(sample);
        const Real alpha=DspCore::Detail::Signal::Alpha(dt,magnitude>value_ ? attack_ : release_);
        const Real next=alpha==Real(1) ? magnitude : value_+alpha*(magnitude-value_);
        if(!DspCore::Detail::Signal::Finite(next)) return false;
        value_=next;output=next;return true;
    }
    Real Value() const noexcept { return value_; }
private:
    Real attack_=Real(0),release_=Real(0),value_=Real(0);
};
}}
#endif
