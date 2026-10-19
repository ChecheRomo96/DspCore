#ifndef DSPCORE_CONTROL_RAMP_H
#define DSPCORE_CONTROL_RAMP_H
#include <DspCore/Control/Detail/Common.h>
namespace DspCore { namespace Control {
/// @ingroup DspCore_Control_Smoothing
/// @brief Linear ramp from current value to a target over an explicit duration in seconds.
template<typename Real=float> class LinearRamp {
public:
    bool Reset(Real value=Real(0)) noexcept {
        if(!Detail::Finite(value)) return false;
        value_=start_=target_=value; duration_=elapsed_=Real(0); return true;
    }
    /// Retarget starts at the current value; zero duration snaps immediately.
    bool SetTarget(Real target, Real seconds) noexcept {
        if(!Detail::Finite(target) || !Detail::Finite(seconds) || seconds<Real(0)) return false;
        start_=value_; target_=target; duration_=seconds; elapsed_=Real(0);
        if(seconds==Real(0)) value_=target;
        return true;
    }
    bool Update(Real dt, Real& output) noexcept {
        if(!Detail::Finite(dt) || dt<=Real(0)) return false;
        const Real elapsed=dt>=duration_-elapsed_ ? duration_ : elapsed_+dt;
        const Real next=elapsed==duration_ ? target_ : Detail::Mix(start_,target_,elapsed/duration_);
        if(!Detail::Finite(next)) return false;
        elapsed_=elapsed; value_=next; output=next; return true;
    }
    Real Value() const noexcept { return value_; }
    Real Target() const noexcept { return target_; }
    Real Remaining() const noexcept { return duration_-elapsed_; }
private:
    Real value_=Real(0), start_=Real(0), target_=Real(0), duration_=Real(0), elapsed_=Real(0);
};
/// @ingroup DspCore_Control_Smoothing
/// @brief Asymmetric rise/fall rates in value units per second; a zero rate holds that direction.
template<typename Real=float> class SlewLimiter {
public:
    bool Configure(Real rise, Real fall) noexcept {
        if(!Detail::Finite(rise) || !Detail::Finite(fall) || rise<Real(0) || fall<Real(0)) return false;
        rise_=rise; fall_=fall; return true;
    }
    bool Reset(Real value=Real(0)) noexcept {
        if(!Detail::Finite(value)) return false;
        value_=value; return true;
    }
    bool Update(Real input, Real dt, Real& output) noexcept {
        if(!Detail::Finite(input) || !Detail::Finite(dt) || dt<=Real(0)) return false;
        const Real delta=input-value_;
        const Real step=(delta>=Real(0) ? rise_ : fall_)*dt;
        if(!Detail::Finite(delta) || !Detail::Finite(step)) return false;
        const Real change=Detail::Clamp(delta,-step,step);
        const Real next=(delta>=-step && delta<=step) ? input : value_+change;
        if(!Detail::Finite(next)) return false;
        value_=next; output=next; return true;
    }
    Real Value() const noexcept { return value_; }
private:
    Real value_=Real(0), rise_=Real(1), fall_=Real(1);
};
}}
#endif
