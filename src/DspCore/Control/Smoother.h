#ifndef DSPCORE_CONTROL_SMOOTHER_H
#define DSPCORE_CONTROL_SMOOTHER_H
#include <DspCore/Control/Detail/Common.h>
namespace DspCore { namespace Control {
/// @ingroup DspCore_Control_Smoothing
/// @brief One-pole smoothing with an explicit time constant and per-call interval in seconds.
template<typename Real=float> class Smoother {
public:
    /// Finite nonnegative seconds; zero bypasses smoothing, preserving current state.
    bool Configure(Real seconds) noexcept {
        if(!Detail::Finite(seconds) || seconds<Real(0)) return false;
        tau_=seconds; return true;
    }
    bool Reset(Real value=Real(0)) noexcept {
        if(!Detail::Finite(value)) return false;
        value_=value; return true;
    }
    /// Invalid input/interval leaves state and output unchanged.
    bool Update(Real input, Real dt, Real& output) noexcept {
        if(!Detail::Finite(input) || !Detail::Finite(dt) || dt<=Real(0)) return false;
        const Real next=Detail::Mix(value_,input,Detail::Alpha(dt,tau_));
        if(!Detail::Finite(next)) return false;
        value_=next; output=next; return true;
    }
    Real Value() const noexcept { return value_; }
    Real TimeConstant() const noexcept { return tau_; }
private:
    Real tau_=Real(0), value_=Real(0);
};
}}
#endif
