#ifndef DSPCORE_FILTERS_IIR_H
#define DSPCORE_FILTERS_IIR_H
#include <CPutility.h>
#include <CPvector.h>
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/ProcessorBase.h>
namespace DspCore { namespace Filters {
/// @ingroup DspCore_Filters
/// @brief Runtime-order SISO ProcessorBase with CPSTL vector storage.
template<typename T=float> class IIR : public DspCore::Processing::ProcessorBase<T> {
public:
    /// The empty default state is a one-pole identity until Configure succeeds.
    IIR() noexcept {}
    /// Normalize by a0 and resize storage outside Process. Failure preserves all state.
    bool Configure(const T* numerator,size_t numeratorCount,
                   const T* denominator,size_t denominatorCount) noexcept {
        if(!numerator || !denominator || numeratorCount==0 || denominatorCount==0 ||
           !DspCore::Detail::Signal::Finite(denominator[0]) || denominator[0]==T(0)) return false;
        for(size_t i=0;i<numeratorCount;++i)
            if(!DspCore::Detail::Signal::Finite(numerator[i])) return false;
        for(size_t i=0;i<denominatorCount;++i)
            if(!DspCore::Detail::Signal::Finite(denominator[i])) return false;

        cpstd::vector<T> nextNumerator;
        cpstd::vector<T> nextDenominator;
        cpstd::vector<T> nextInputHistory;
        cpstd::vector<T> nextOutputHistory;
        nextNumerator.resize(numeratorCount);
        nextDenominator.resize(denominatorCount);
        nextInputHistory.resize(numeratorCount-1);
        nextOutputHistory.resize(denominatorCount-1);
        if(nextNumerator.size()!=numeratorCount || nextDenominator.size()!=denominatorCount ||
           nextInputHistory.size()!=numeratorCount-1 || nextOutputHistory.size()!=denominatorCount-1) return false;
        for(size_t i=0;i<numeratorCount;++i) {
            nextNumerator[i]=numerator[i]/denominator[0];
            if(!DspCore::Detail::Signal::Finite(nextNumerator[i])) return false;
        }
        for(size_t i=0;i<denominatorCount;++i) {
            nextDenominator[i]=denominator[i]/denominator[0];
            if(!DspCore::Detail::Signal::Finite(nextDenominator[i])) return false;
        }
        numerator_=cpstd::move(nextNumerator);
        denominator_=cpstd::move(nextDenominator);
        inputHistory_=cpstd::move(nextInputHistory);
        outputHistory_=cpstd::move(nextOutputHistory);
        return true;
    }
    template<size_t B,size_t A> bool Configure(const T (&b)[B],const T (&a)[A]) noexcept {
        return Configure(b,B,a,A);
    }
    /// Accept a coefficient struct with numerator/denominator arrays, e.g. Biquad::Coefficients<T>.
    template<typename C> bool Configure(const C& coefficients) noexcept {
        return Configure(coefficients.numerator,coefficients.denominator);
    }
    void Reset() noexcept override {
        for(size_t i=0;i<inputHistory_.size();++i) inputHistory_[i]=T(0);
        for(size_t i=0;i<outputHistory_.size();++i) outputHistory_[i]=T(0);
    }
private:
    /** @brief Always returns one: IIR has exactly one signal input. */
    size_t InputPorts() const noexcept override { return 1; }
    /** @brief Always returns one: IIR has exactly one signal output. */
    size_t OutputPorts() const noexcept override { return 1; }
public:
    /**
     * @brief Processes @p sampleCount input values into equally sized output storage.
     *
     * Both pointers may be null only for a zero-length block. On success every
     * output value is valid. Invalid views or non-finite input leave state and
     * output unchanged. A finite arithmetic failure may commit an output
     * prefix before returning @c false.
     */
    bool Process(const T* input,T* output,size_t sampleCount) noexcept {
        DspCore::Processing::InputBlock<T> inputs(input,sampleCount);
        DspCore::Processing::OutputBlock<T> outputs(output,sampleCount);
        return Process(&inputs,1,&outputs,1) && outputs.size==sampleCount;
    }
private:
    /// SISO adapter used only by ProcessorBase graphs.
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
public:
    bool GetNumerator(size_t index,T& value) const noexcept {
        if(numerator_.empty()) {
            if(index!=0) return false;
            value=T(1);
            return true;
        }
        if(index>=numerator_.size()) return false;
        value=numerator_[index];
        return true;
    }
    bool GetDenominator(size_t index,T& value) const noexcept {
        if(denominator_.empty()) {
            if(index!=0) return false;
            value=T(1);
            return true;
        }
        if(index>=denominator_.size()) return false;
        value=denominator_[index];
        return true;
    }
    size_t NumeratorCount() const noexcept { return numerator_.empty() ? 1U : numerator_.size(); }
    size_t DenominatorCount() const noexcept { return denominator_.empty() ? 1U : denominator_.size(); }
    size_t NumeratorCapacity() const noexcept { return numerator_.capacity(); }
    size_t DenominatorCapacity() const noexcept { return denominator_.capacity(); }
private:
    /// Direct-form I recurrence; no pole inference, clipping or allocation.
    bool ProcessSample(T input,T& output) noexcept {
        if(!DspCore::Detail::Signal::Finite(input)) return false;
        if(numerator_.empty()) { output=input; return true; }
        T sample=numerator_[0]*input;
        if(!DspCore::Detail::Signal::Finite(sample)) return false;
        for(size_t i=1;i<numerator_.size();++i) {
            sample+=numerator_[i]*inputHistory_[i-1];
            if(!DspCore::Detail::Signal::Finite(sample)) return false;
        }
        for(size_t i=1;i<denominator_.size();++i) {
            sample-=denominator_[i]*outputHistory_[i-1];
            if(!DspCore::Detail::Signal::Finite(sample)) return false;
        }
        for(size_t i=inputHistory_.size();i>1;--i) inputHistory_[i-1]=inputHistory_[i-2];
        for(size_t i=outputHistory_.size();i>1;--i) outputHistory_[i-1]=outputHistory_[i-2];
        if(!inputHistory_.empty()) inputHistory_[0]=input;
        if(!outputHistory_.empty()) outputHistory_[0]=sample;
        output=sample;
        return true;
    }
    cpstd::vector<T> numerator_;
    cpstd::vector<T> denominator_;
    cpstd::vector<T> inputHistory_;
    cpstd::vector<T> outputHistory_;
};
}}
#endif
