#ifndef DSPCORE_ANALYSIS_STREAMING_H
#define DSPCORE_ANALYSIS_STREAMING_H

#include <DspCore/Detail/SignalMath.h>
#include <DspCore/LUT/LUT1D.h>

namespace DspCore { namespace Analysis { namespace Streaming {

/// Rectified one-pole amplitude follower with separate attack/release seconds.
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

/**
 * @brief Exact moving RMS with one result per sample and a fixed sample window.
 *
 * Configure allocates the history once. Update reuses its running sum and
 * ring slot, so it performs constant work and no allocation. Before the ring
 * is full, output uses the available sample count; IsReady reports when the
 * configured window is complete.
 */
template<typename Real=float> class MovingRMS {
public:
    bool Configure(cpstd::size_t windowSamples) noexcept {
        if(windowSamples==0U || !history_.Resize(windowSamples)) return false;
        window_=windowSamples; return Reset();
    }
    bool Reset() noexcept {
        if(window_==0U) return false;
        history_.Fill(Real(0)); write_=0U; count_=0U; sumSquares_=Real(0); return true;
    }
    bool IsReady() const noexcept { return window_!=0U && count_==window_; }
    cpstd::size_t WindowSamples() const noexcept { return window_; }
    cpstd::size_t AvailableSamples() const noexcept { return count_; }

    bool Update(Real sample,Real& output) noexcept {
        if(window_==0U || !DspCore::Detail::Signal::Finite(sample)) return false;
        const Real square=sample*sample;
        if(!DspCore::Detail::Signal::Finite(square)) return false;
        const Real old=history_.GetValue(write_);
        const Real nextSum=sumSquares_+square-old;
        if(!DspCore::Detail::Signal::Finite(nextSum)) return false;
        const cpstd::size_t nextCount=count_<window_ ? count_+1U : count_;
        const Real mean=nextSum/static_cast<Real>(nextCount);
        if(!DspCore::Detail::Signal::Finite(mean) || mean<Real(0)) return false;
        const Real nextOutput=DspCore::Detail::Signal::Sqrt(mean);
        if(!DspCore::Detail::Signal::Finite(nextOutput)) return false;
        if(!history_.SetValue(write_,square)) return false;
        write_=write_+1U==window_ ? 0U : write_+1U;
        count_=nextCount; sumSquares_=nextSum; output=nextOutput; return true;
    }
private:
    LUT::LUT1D<Real> history_;
    cpstd::size_t window_{};
    cpstd::size_t write_{};
    cpstd::size_t count_{};
    Real sumSquares_{};
};

}}} // namespace DspCore::Analysis::Streaming

#endif
