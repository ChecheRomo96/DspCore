#ifndef DSPCORE_CONTROL_PID_H
#define DSPCORE_CONTROL_PID_H
#include <DspCore/Control/Detail/Common.h>
namespace DspCore { namespace Control {
/// @ingroup DspCore_Control_PID
/// @brief Gains use output/error units; integral and derivative gains include seconds.
template<typename Real=float> struct PidParameters {
    Real kp=Real(1), ki=Real(0), kd=Real(0);
    Real outputMin=Real(-1), outputMax=Real(1), derivativeTimeConstant=Real(0);
};
/// @ingroup DspCore_Control_PID
/// @brief Position-form PID, derivative on measurement, explicit dt, conditional-integration anti-windup.
template<typename Real=float> class PID {
public:
    bool Configure(const PidParameters<Real>& p) noexcept {
        if(!Detail::Finite(p.kp) || !Detail::Finite(p.ki) || !Detail::Finite(p.kd) ||
           p.kp<Real(0) || p.ki<Real(0) || p.kd<Real(0) || !Detail::Finite(p.outputMin) ||
           !Detail::Finite(p.outputMax) || !(p.outputMin<p.outputMax) ||
           !Detail::Finite(p.derivativeTimeConstant) || p.derivativeTimeConstant<Real(0)) return false;
        parameters_=p;
        integral_=Detail::Clamp(integral_,p.outputMin,p.outputMax);
        output_=Detail::Clamp(output_,p.outputMin,p.outputMax);
        return true;
    }
    /// Reset derivative history; finite requested integral is clamped to output limits.
    bool Reset(Real integral=Real(0)) noexcept {
        if(!Detail::Finite(integral)) return false;
        integral_=Detail::Clamp(integral,parameters_.outputMin,parameters_.outputMax);
        output_=integral_; previous_=derivative_=Real(0); hasPrevious_=false; return true;
    }
    /// Failure preserves all state and output. dt is positive seconds, not sample count.
    bool Update(Real setpoint, Real measurement, Real dt, Real& output) noexcept {
        if(!Detail::Finite(setpoint) || !Detail::Finite(measurement) || !Detail::Finite(dt) || dt<=Real(0)) return false;
        const Real error=setpoint-measurement;
        if(!Detail::Finite(error)) return false;
        const Real p=parameters_.kp*error;
        Real rate=Real(0);
        if(hasPrevious_ && parameters_.kd!=Real(0)) {
            rate=(measurement-previous_)/dt;
            if(!Detail::Finite(rate)) return false;
        }
        const Real derivative=Detail::Mix(derivative_,rate,Detail::Alpha(dt,parameters_.derivativeTimeConstant));
        const Real d=-parameters_.kd*derivative;
        const Real increment=(parameters_.ki*error)*dt;
        if(!Detail::Finite(p) || !Detail::Finite(d) || !Detail::Finite(increment)) return false;
        const Real sum=integral_+increment;
        if(!Detail::Finite(sum)) return false;
        Real integral=Detail::Clamp(sum,parameters_.outputMin,parameters_.outputMax);
        Real candidate=p+integral+d;
        if(!Detail::Finite(candidate)) return false;
        if((candidate>parameters_.outputMax && integral>integral_) ||
           (candidate<parameters_.outputMin && integral<integral_)) {
            integral=integral_;
            candidate=p+integral+d;
        }
        if(!Detail::Finite(candidate)) return false;
        const Real next=Detail::Clamp(candidate,parameters_.outputMin,parameters_.outputMax);
        integral_=integral; derivative_=derivative; previous_=measurement;
        hasPrevious_=true; output_=next; output=next; return true;
    }
    Real Output() const noexcept { return output_; }
    Real Integral() const noexcept { return integral_; }
    Real MeasurementDerivative() const noexcept { return derivative_; }
    PidParameters<Real> Parameters() const noexcept { return parameters_; }
private:
    PidParameters<Real> parameters_;
    Real integral_=Real(0), previous_=Real(0), derivative_=Real(0), output_=Real(0);
    bool hasPrevious_=false;
};
}}
#endif
