#ifndef DSPCORE_GENERATORS_LUT_SEGMENT_H
#define DSPCORE_GENERATORS_LUT_SEGMENT_H
#include <DspCore/Generators/Envelopes.h>
#include <DspCore/LUT/Sampling.h>
namespace DspCore { namespace Generators { namespace Envelopes {
/// @ingroup DspCore_Generators_Envelopes
/// Fill from a normalized shape table; reject overlapping storage before writing.
/// Multi-sample shapes must start at zero and end at one. A singleton shape is constant.
template<typename Real>
bool LUTSegment(Real* out, Real start, Real end, size_t total, const LUT::LUT1D<Real>& table,
                size_t first=0, size_t last=static_cast<size_t>(-1)) noexcept {
    if (!Generators::Detail::Buffer(out,total) || !Generators::Detail::Finite(start) ||
        !Generators::Detail::Finite(end) || !Generators::Detail::Range(total,first,last) || table.Empty()) return false;
    if (table.Size()>1 && (table.GetValue(0)!=Real(0) || table.GetValue(table.Size()-1)!=Real(1))) return false;
    for(size_t i=0;i<table.Size();++i) {
        const Real x=table.GetValue(i);
        if (!Generators::Detail::Finite(x) || x<Real(0) || x>Real(1)) return false;
    }
    Real shape=Real(0);
    if (!LUT::Sample(table,Real(0.5),shape,LUT::Boundary::Reject)) return false;
    if (total!=0) {
        const uintptr_t a=reinterpret_cast<uintptr_t>(out), b=reinterpret_cast<uintptr_t>(table.GetBuffer());
        const size_t outBytes=total*sizeof(Real), inBytes=table.Size()*sizeof(Real);
        if ((a<=b && b-a<outBytes) || (b<a && a-b<inBytes)) return false;
    }
    const size_t length=last-first;
    for(size_t i=0;i<length;++i) {
        if (length==1) { out[first]=start; break; }
        const Real t=static_cast<Real>(i)/static_cast<Real>(length-1);
        // Validation above guarantees this read succeeds; no table mutation occurs.
        LUT::Sample(table,t,shape,LUT::Boundary::Reject);
        out[first+i]=Generators::Detail::Mix(start,end,shape);
    }
    return true;
}
}}}
#endif
