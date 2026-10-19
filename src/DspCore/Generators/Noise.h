#ifndef DSPCORE_GENERATORS_NOISE_H
#define DSPCORE_GENERATORS_NOISE_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Noise {
/// @ingroup DspCore_Generators_Noise
/// Xorshift32 white noise in offset+[-amplitude,amplitude]; state must be nonzero.
/// Passing the updated state across blocks preserves sequence continuity.
template<typename Real>
bool White(Real* out, size_t count, uint32_t& state, Real amplitude=Real(1), Real offset=Real(0)) noexcept {
    if (!Detail::Buffer(out,count) || !Detail::Gain(amplitude,offset) || state==0) return false;
    for(size_t i=0;i<count;++i) {
        state ^= state << 13;
        state ^= state >> 17;
        state ^= state << 5;
        const Real unit=static_cast<Real>(state)/static_cast<Real>(UINT32_MAX);
        out[i]=offset+amplitude*(Real(2)*unit-Real(1));
    }
    return true;
}
}}}
#endif
