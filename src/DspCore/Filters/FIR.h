#ifndef DSPCORE_FILTERS_FIR_H
#define DSPCORE_FILTERS_FIR_H
#include <DspCore/Detail/SignalMath.h>
namespace DspCore { namespace Filters {
/// @ingroup DspCore_Filters
/// @brief Fixed-tap FIR with inline coefficients/history and transactional sample updates.
template<typename T=float,size_t TapCount=1> class FIR {
    static_assert(TapCount>0,"FIR requires at least one tap");
public:
    FIR() noexcept { coefficients_[0]=T(1); }
    /// b[0] multiplies the current sample. A valid configuration resets history.
    bool Configure(const T* coefficients,size_t count) noexcept {
        if(!coefficients || count!=TapCount) return false;
        for(size_t i=0;i<TapCount;++i) if(!DspCore::Detail::Signal::Finite(coefficients[i])) return false;
        for(size_t i=0;i<TapCount;++i) coefficients_[i]=coefficients[i];
        Reset(); return true;
    }
    void Reset() noexcept {
        for(size_t i=0;i<TapCount;++i) history_[i]=T(0);
        next_=0;
    }
    /// Failure leaves delay history and output unchanged; no clipping is applied.
    bool Process(T input,T& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(input)) return false;
        T sum=coefficients_[0]*input;
        if(!DspCore::Detail::Signal::Finite(sum)) return false;
        size_t index=next_;
        for(size_t tap=1;tap<TapCount;++tap) {
            index=index==0 ? TapCount-1 : index-1;
            sum+=coefficients_[tap]*history_[index];
            if(!DspCore::Detail::Signal::Finite(sum)) return false;
        }
        history_[next_]=input;
        next_=next_==TapCount-1 ? 0 : next_+1;
        output=sum; return true;
    }
    static constexpr size_t Taps() noexcept { return TapCount; }
private:
    T coefficients_[TapCount]={};
    T history_[TapCount]={};
    size_t next_=0;
};
}}
#endif
