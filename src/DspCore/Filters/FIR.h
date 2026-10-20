#ifndef DSPCORE_FILTERS_FIR_H
#define DSPCORE_FILTERS_FIR_H
#include <CPutility.h>
#include <CPvector.h>
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/ProcessorBase.h>
namespace DspCore { namespace Filters {
/// @ingroup DspCore_Filters
/// @brief Runtime-tap SISO ProcessorBase with CPSTL vector storage.
template<typename T=float> class FIR : public DspCore::Processing::ProcessorBase<T> {
public:
    /// The empty default state is a one-tap identity until Configure succeeds.
    FIR() noexcept {}
    /// b[0] multiplies the current sample. Storage grows or shrinks outside Process.
    bool Configure(const T* coefficients,size_t count) noexcept {
        if(!coefficients || count==0) return false;
        for(size_t i=0;i<count;++i) if(!DspCore::Detail::Signal::Finite(coefficients[i])) return false;
        cpstd::vector<T> nextCoefficients;
        cpstd::vector<T> nextHistory;
        nextCoefficients.resize(count);
        nextHistory.resize(count);
        if(nextCoefficients.size()!=count || nextHistory.size()!=count) return false;
        for(size_t i=0;i<count;++i) nextCoefficients[i]=coefficients[i];
        coefficients_=cpstd::move(nextCoefficients);
        history_=cpstd::move(nextHistory);
        next_=0;
        return true;
    }
    void Reset() noexcept override {
        for(size_t i=0;i<history_.size();++i) history_[i]=T(0);
        next_=0;
    }
    /** @brief Always returns one: FIR has exactly one signal input. */
    size_t InputPorts() const noexcept override { return 1; }
    /** @brief Always returns one: FIR has exactly one signal output. */
    size_t OutputPorts() const noexcept override { return 1; }
    /// Processes every input value and writes exactly one output value for each.
    bool Process(const DspCore::Processing::InputBlock<T>* inputs,size_t inputCount,
                 DspCore::Processing::OutputBlock<T>* outputs,size_t outputCount) noexcept override {
        if(inputCount!=1 || outputCount!=1 || !inputs || !outputs || !inputs[0].IsValid() ||
           !outputs[0].CanWrite(inputs[0].size)) return false;
        for(size_t i=0;i<inputs[0].size;++i)
            if(!DspCore::Detail::Signal::Finite(inputs[0].data[i])) return false;
        for(size_t i=0;i<inputs[0].size;++i) {
            if(!ProcessSample(inputs[0].data[i],outputs[0].data[i])) {
                outputs[0].size=i;
                return false;
            }
        }
        outputs[0].size=inputs[0].size;
        return true;
    }
    size_t Taps() const noexcept { return coefficients_.empty() ? 1U : coefficients_.size(); }
    size_t Capacity() const noexcept { return coefficients_.capacity(); }
private:
    /// Failure leaves delay history and output unchanged; no clipping is applied.
    bool ProcessSample(T input,T& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(input)) return false;
        if(coefficients_.empty()) { output=input; return true; }
        T sum=coefficients_[0]*input;
        if(!DspCore::Detail::Signal::Finite(sum)) return false;
        size_t index=next_;
        for(size_t tap=1;tap<coefficients_.size();++tap) {
            index=index==0 ? coefficients_.size()-1 : index-1;
            sum+=coefficients_[tap]*history_[index];
            if(!DspCore::Detail::Signal::Finite(sum)) return false;
        }
        history_[next_]=input;
        next_=next_==coefficients_.size()-1 ? 0 : next_+1;
        output=sum;
        return true;
    }
    cpstd::vector<T> coefficients_;
    cpstd::vector<T> history_;
    size_t next_=0;
};
}}
#endif
