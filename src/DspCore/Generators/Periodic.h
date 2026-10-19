#ifndef DSPCORE_GENERATORS_PERIODIC_H
#define DSPCORE_GENERATORS_PERIODIC_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Periodic {
/// @cond DSPCORE_INTERNAL
namespace Detail {
enum class Wave { Sine, Saw, Square, Triangle };
template<typename Real>
bool Generate(Real* buffer, size_t count, Real amplitude, Real offset, Real phase, Wave wave) noexcept {
    if (!Generators::Detail::Buffer(buffer,count) || !Generators::Detail::Gain(amplitude,offset) ||
        !Generators::Detail::Finite(phase)) return false;
    const Real start=Generators::Detail::Wrap(phase);
    for (size_t i=0;i<count;++i) {
        Real p=Generators::Detail::Wrap(start+static_cast<Real>(i)/static_cast<Real>(count));
        Real sample=Real(0);
        switch(wave) {
            case Wave::Sine: sample=Generators::Detail::Sin(Real(2)*Generators::Detail::Pi<Real>()*p); break;
            case Wave::Saw: sample=Real(2)*Generators::Detail::Wrap(p+Real(0.5))-Real(1); break;
            case Wave::Square: sample=p < Real(0.5) ? Real(1) : Real(-1); break;
            case Wave::Triangle:
                p=Generators::Detail::Wrap(p+Real(0.75));
                sample=Real(2)*Generators::Detail::Abs(Real(2)*p-Real(1))-Real(1); break;
        }
        buffer[i]=offset+amplitude*sample;
    }
    return true;
}
}
/// @endcond
/// @ingroup DspCore_Generators_Periodic
/// One sine cycle without a duplicate endpoint; phase is in cycles.
template<typename Real> bool Sine(Real* out, size_t count, Real amplitude=Real(1), Real offset=Real(0), Real phase=Real(0)) noexcept {
    return Detail::Generate(out,count,amplitude,offset,phase,Detail::Wave::Sine);
}
/// @ingroup DspCore_Generators_Periodic
/// Rising saw starts at zero with phase zero, with its discontinuity at half-cycle.
template<typename Real> bool Saw(Real* out, size_t count, Real amplitude=Real(1), Real offset=Real(0), Real phase=Real(0)) noexcept {
    return Detail::Generate(out,count,amplitude,offset,phase,Detail::Wave::Saw);
}
/// @ingroup DspCore_Generators_Periodic
/// Square starts positive at phase zero; phase 0.5 selects the negative half.
template<typename Real> bool Square(Real* out, size_t count, Real amplitude=Real(1), Real offset=Real(0), Real phase=Real(0)) noexcept {
    return Detail::Generate(out,count,amplitude,offset,phase,Detail::Wave::Square);
}
/// @ingroup DspCore_Generators_Periodic
/// Triangle starts at zero with a rising slope, matching the historical convention.
template<typename Real> bool Triangle(Real* out, size_t count, Real amplitude=Real(1), Real offset=Real(0), Real phase=Real(0)) noexcept {
    return Detail::Generate(out,count,amplitude,offset,phase,Detail::Wave::Triangle);
}
}}}
#endif
