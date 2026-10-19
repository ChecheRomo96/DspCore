#ifndef DSPCORE_CONTROL_MAPPING_H
#define DSPCORE_CONTROL_MAPPING_H
#include <DspCore/Control/Detail/Common.h>
namespace DspCore { namespace Control {
/// @ingroup DspCore_Control_Mapping
/// Explicit handling of an input beyond its mapping domain.
enum class MappingBoundary : unsigned char { Clamp, Reject, Extrapolate };
/// @ingroup DspCore_Control_Mapping
/// Linear mapping of an ascending input range to either output direction.
template<typename Real>
bool Map(Real input, Real inputLow, Real inputHigh, Real outputLow, Real outputHigh,
         Real& output, MappingBoundary boundary) noexcept {
    if(!Detail::Finite(input) || !Detail::Finite(inputLow) || !Detail::Finite(inputHigh) ||
       !Detail::Finite(outputLow) || !Detail::Finite(outputHigh) || !(inputLow<inputHigh)) return false;
    if(boundary!=MappingBoundary::Clamp && boundary!=MappingBoundary::Reject && boundary!=MappingBoundary::Extrapolate) return false;
    const Real span=inputHigh-inputLow;
    if(!Detail::Finite(span)) return false;
    if(boundary==MappingBoundary::Reject && (input<inputLow || input>inputHigh)) return false;
    if(boundary==MappingBoundary::Clamp) input=Detail::Clamp(input,inputLow,inputHigh);
    const Real t=(input-inputLow)/span;
    if(!Detail::Finite(t)) return false;
    const Real next=Detail::Mix(outputLow,outputHigh,t);
    if(!Detail::Finite(next)) return false;
    output=next; return true;
}
/// @ingroup DspCore_Control_Mapping
/// Continuous symmetric deadband: zero inside +/-width, subtract width outside.
template<typename Real> bool Deadband(Real input, Real width, Real& output) noexcept {
    if(!Detail::Finite(input) || !Detail::Finite(width) || width<Real(0)) return false;
    output=input>width ? input-width : (input<-width ? input+width : Real(0));
    return true;
}
/// @ingroup DspCore_Control_Mapping
/// @brief Schmitt trigger with inclusive lower/upper thresholds and a latched state.
template<typename Real=float> class Hysteresis {
public:
    bool Configure(Real lower, Real upper) noexcept {
        if(!Detail::Finite(lower) || !Detail::Finite(upper) || !(lower<upper)) return false;
        lower_=lower; upper_=upper; return true;
    }
    void Reset(bool active=false) noexcept { active_=active; }
    bool Update(Real input, bool& output) noexcept {
        if(!Detail::Finite(input)) return false;
        if(input>=upper_) active_=true;
        else if(input<=lower_) active_=false;
        output=active_; return true;
    }
    bool Active() const noexcept { return active_; }
private:
    Real lower_=Real(0), upper_=Real(1);
    bool active_=false;
};
}}
#endif
