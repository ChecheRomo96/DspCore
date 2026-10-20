#ifndef DSPCORE_FILTERS_FIR_H
#define DSPCORE_FILTERS_FIR_H
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/ProcessorBase.h>
namespace DspCore { namespace Filters {
/// @ingroup DspCore_Filters
/// @brief Runtime-tap SISO ProcessorBase with fixed-capacity inline storage.
template<typename T=float,size_t MaximumTapCount=1> class FIR : public DspCore::Processing::ProcessorBase<T> {
    static_assert(MaximumTapCount>0,"FIR requires positive tap capacity");
public:
    FIR() noexcept { coefficients_[0]=T(1); }
    /// b[0] multiplies the current sample. Count may change up to MaximumTaps().
    bool Configure(const T* coefficients,size_t count) noexcept {
        if(!coefficients || count==0 || count>MaximumTapCount) return false;
        for(size_t i=0;i<count;++i) if(!DspCore::Detail::Signal::Finite(coefficients[i])) return false;
        for(size_t i=0;i<count;++i) coefficients_[i]=coefficients[i];
        tapCount_=count;
        Reset(); return true;
    }
    void Reset() noexcept override {
        for(size_t i=0;i<MaximumTapCount;++i) history_[i]=T(0);
        next_=0;
    }
    /// Failure leaves delay history and output unchanged; no clipping is applied.
    bool Process(T input,T& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(input)) return false;
        T sum=coefficients_[0]*input;
        if(!DspCore::Detail::Signal::Finite(sum)) return false;
        size_t index=next_;
        for(size_t tap=1;tap<tapCount_;++tap) {
            index=index==0 ? tapCount_-1 : index-1;
            sum+=coefficients_[tap]*history_[index];
            if(!DspCore::Detail::Signal::Finite(sum)) return false;
        }
        history_[next_]=input;
        next_=next_==tapCount_-1 ? 0 : next_+1;
        output=sum; return true;
    }
    size_t InputPorts() const noexcept override { return 1; }
    size_t OutputPorts() const noexcept override { return 1; }
    /// Process one SISO block transactionally; direct sample processing remains available above.
    bool Process(const DspCore::Processing::InputBlock<T>* inputs,size_t inputCount,
                 DspCore::Processing::OutputBlock<T>* outputs,size_t outputCount) noexcept override {
        if(inputCount!=1 || outputCount!=1 || !inputs || !outputs || !inputs[0].IsValid() ||
           !outputs[0].CanWrite(inputs[0].size)) return false;
        FIR probe(*this);
        T sample=T(0);
        for(size_t i=0;i<inputs[0].size;++i)
            if(!probe.Process(inputs[0].data[i],sample)) return false;
        for(size_t i=0;i<inputs[0].size;++i)
            if(!Process(inputs[0].data[i],outputs[0].data[i])) return false;
        outputs[0].size=inputs[0].size;
        return true;
    }
    size_t Taps() const noexcept { return tapCount_; }
    static constexpr size_t MaximumTaps() noexcept { return MaximumTapCount; }
private:
    T coefficients_[MaximumTapCount]={};
    T history_[MaximumTapCount]={};
    size_t next_=0;
    size_t tapCount_=1;
};
}}
#endif
