#ifndef DSPCORE_FILTERS_IIR_H
#define DSPCORE_FILTERS_IIR_H
#include <DspCore/Detail/SignalMath.h>
namespace DspCore { namespace Filters {
/// @ingroup DspCore_Filters
/// @brief Generic fixed-size IIR processor; coefficient counts are selected at compile time.
template<typename T=float,size_t NumeratorCount=3,size_t DenominatorCount=3> class IIR {
    static_assert(NumeratorCount>0 && DenominatorCount>0,"IIR requires numerator and denominator coefficients");
    static constexpr size_t InputHistory=NumeratorCount>1 ? NumeratorCount-1 : 1;
    static constexpr size_t OutputHistory=DenominatorCount>1 ? DenominatorCount-1 : 1;
public:
    IIR() noexcept { numerator_[0]=denominator_[0]=T(1); }
    /// Normalize by a0 after full validation, then reset history. Failure preserves all state.
    bool Configure(const T* numerator,size_t numeratorCount,const T* denominator,size_t denominatorCount) noexcept {
        if(!numerator || !denominator || numeratorCount!=NumeratorCount || denominatorCount!=DenominatorCount ||
           !DspCore::Detail::Signal::Finite(denominator[0]) || denominator[0]==T(0)) return false;
        T b[NumeratorCount]={},a[DenominatorCount]={};
        for(size_t i=0;i<NumeratorCount;++i) {
            if(!DspCore::Detail::Signal::Finite(numerator[i])) return false;
            b[i]=numerator[i]/denominator[0];
            if(!DspCore::Detail::Signal::Finite(b[i])) return false;
        }
        for(size_t i=0;i<DenominatorCount;++i) {
            if(!DspCore::Detail::Signal::Finite(denominator[i])) return false;
            a[i]=denominator[i]/denominator[0];
            if(!DspCore::Detail::Signal::Finite(a[i])) return false;
        }
        for(size_t i=0;i<NumeratorCount;++i) numerator_[i]=b[i];
        for(size_t i=0;i<DenominatorCount;++i) denominator_[i]=a[i];
        Reset();return true;
    }
    template<size_t B,size_t A> bool Configure(const T (&b)[B],const T (&a)[A]) noexcept {
        return Configure(b,B,a,A);
    }
    /// Accept a coefficient struct with numerator/denominator arrays, e.g. Biquad::Coefficients<T>.
    template<typename C> bool Configure(const C& coefficients) noexcept {
        return Configure(coefficients.numerator,coefficients.denominator);
    }
    void Reset() noexcept {
        for(size_t i=0;i<InputHistory;++i) input_[i]=T(0);
        for(size_t i=0;i<OutputHistory;++i) output_[i]=T(0);
    }
    /// Direct-form I recurrence; no pole inference, clipping or allocation.
    bool Process(T input,T& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(input)) return false;
        T sample=numerator_[0]*input;
        if(!DspCore::Detail::Signal::Finite(sample)) return false;
        for(size_t i=1;i<NumeratorCount;++i) {
            sample+=numerator_[i]*input_[i-1];
            if(!DspCore::Detail::Signal::Finite(sample)) return false;
        }
        for(size_t i=1;i<DenominatorCount;++i) {
            sample-=denominator_[i]*output_[i-1];
            if(!DspCore::Detail::Signal::Finite(sample)) return false;
        }
        for(size_t i=InputHistory-1;i>0;--i) input_[i]=input_[i-1];
        for(size_t i=OutputHistory-1;i>0;--i) output_[i]=output_[i-1];
        input_[0]=input;output_[0]=sample;output=sample;return true;
    }
    bool GetNumerator(size_t index,T& value) const noexcept {
        if(index>=NumeratorCount) return false;
        value=numerator_[index];return true;
    }
    bool GetDenominator(size_t index,T& value) const noexcept {
        if(index>=DenominatorCount) return false;
        value=denominator_[index];return true;
    }
private:
    T numerator_[NumeratorCount]={},denominator_[DenominatorCount]={};
    T input_[InputHistory]={},output_[OutputHistory]={};
};
}}
#endif
