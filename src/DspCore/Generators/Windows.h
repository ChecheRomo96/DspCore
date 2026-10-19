#ifndef DSPCORE_GENERATORS_WINDOWS_H
#define DSPCORE_GENERATORS_WINDOWS_H
#include <DspCore/Generators/Detail/Common.h>
namespace DspCore { namespace Generators { namespace Windows {
/// @cond DSPCORE_INTERNAL
namespace Detail {
template<typename Real>
bool Generate(Real* out, size_t count, bool hann) noexcept {
    if (!Generators::Detail::Buffer(out,count) || !Generators::Detail::Finite(Real(0))) return false;
    for(size_t i=0;i<count;++i) {
        if(count==1) { out[i]=Real(1); break; }
        if(i==0 || i==count-1) { out[i]=hann ? Real(0) : Real(0.08); continue; }
        const Real angle=Real(2)*Generators::Detail::Pi<Real>()*static_cast<Real>(i)/static_cast<Real>(count-1);
        const Real cosine=Generators::Detail::Cos(angle);
        out[i]=hann ? Real(0.5)-Real(0.5)*cosine : Real(0.54)-Real(0.46)*cosine;
    }
    return true;
}
}
/// @endcond
/// @ingroup DspCore_Generators_Windows
/// Symmetric Hann (N-1 denominator), endpoints zero; singleton value one.
template<typename Real> bool Hann(Real* out, size_t count) noexcept { return Detail::Generate(out,count,true); }
/// @ingroup DspCore_Generators_Windows
/// Symmetric Hamming (N-1 denominator), endpoints 0.08; singleton value one.
template<typename Real> bool Hamming(Real* out, size_t count) noexcept { return Detail::Generate(out,count,false); }
}}}
#endif
