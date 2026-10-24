#ifndef DSPCORE_FILTERS_IIR_H
#define DSPCORE_FILTERS_IIR_H

#include <CPutility.h>
#include <CPvector.h>
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/ProcessorBase.h>

namespace DspCore::Filters {

/**
 * @brief Runtime-order direct-form-I infinite impulse response SISO processor.
 *
 * The stream's SampleType is intentionally independent from both coefficient
 * storage and recurrence StateType. StateType defaults to CoefficientType.
 */
template <typename SampleType = float, typename CoefficientType = float, typename StateType = CoefficientType>
class IIR final : public Processing::ProcessorBase<SampleType> {
public:
    /** @brief Owns numerator and denominator arrays for a generic IIR recurrence. */
    struct Coefficients {
        cpstd::vector<CoefficientType> numerator;
        cpstd::vector<CoefficientType> denominator;

        bool Assign(const CoefficientType* numeratorValues, const cpstd::size_t numeratorCount,
            const CoefficientType* denominatorValues, const cpstd::size_t denominatorCount) noexcept {
            if (numeratorValues == nullptr || denominatorValues == nullptr || numeratorCount == 0U ||
                denominatorCount == 0U) {
                return false;
            }
            cpstd::vector<CoefficientType> nextNumerator;
            cpstd::vector<CoefficientType> nextDenominator;
            nextNumerator.resize(numeratorCount);
            nextDenominator.resize(denominatorCount);
            if (nextNumerator.size() != numeratorCount || nextDenominator.size() != denominatorCount) {
                return false;
            }
            for (cpstd::size_t index = 0U; index < numeratorCount; ++index) {
                if (!DspCore::Detail::Signal::Finite(numeratorValues[index])) {
                    return false;
                }
                nextNumerator[index] = numeratorValues[index];
            }
            for (cpstd::size_t index = 0U; index < denominatorCount; ++index) {
                if (!DspCore::Detail::Signal::Finite(denominatorValues[index])) {
                    return false;
                }
                nextDenominator[index] = denominatorValues[index];
            }
            numerator = cpstd::move(nextNumerator);
            denominator = cpstd::move(nextDenominator);
            return true;
        }
    };

    /** @brief Copies and normalizes raw `b[]` and `a[]` coefficients by `a[0]`. */
    bool Configure(const CoefficientType* numerator, const cpstd::size_t numeratorCount,
        const CoefficientType* denominator, const cpstd::size_t denominatorCount) noexcept {
        if (numerator == nullptr || denominator == nullptr || numeratorCount == 0U || denominatorCount == 0U ||
            !DspCore::Detail::Signal::Finite(denominator[0]) || denominator[0] == CoefficientType{}) {
            return false;
        }

        cpstd::vector<CoefficientType> nextNumerator;
        cpstd::vector<CoefficientType> nextDenominator;
        cpstd::vector<StateType> nextInputHistory;
        cpstd::vector<StateType> nextOutputHistory;
        nextNumerator.resize(numeratorCount);
        nextDenominator.resize(denominatorCount);
        nextInputHistory.resize(numeratorCount - 1U);
        nextOutputHistory.resize(denominatorCount - 1U);
        if (nextNumerator.size() != numeratorCount || nextDenominator.size() != denominatorCount ||
            nextInputHistory.size() != numeratorCount - 1U || nextOutputHistory.size() != denominatorCount - 1U) {
            return false;
        }

        for (cpstd::size_t index = 0U; index < numeratorCount; ++index) {
            const auto normalized = numerator[index] / denominator[0];
            if (!DspCore::Detail::Signal::Finite(numerator[index]) || !DspCore::Detail::Signal::Finite(normalized)) {
                return false;
            }
            nextNumerator[index] = normalized;
        }
        for (cpstd::size_t index = 0U; index < denominatorCount; ++index) {
            const auto normalized = denominator[index] / denominator[0];
            if (!DspCore::Detail::Signal::Finite(denominator[index]) || !DspCore::Detail::Signal::Finite(normalized)) {
                return false;
            }
            nextDenominator[index] = normalized;
        }

        numerator_ = cpstd::move(nextNumerator);
        denominator_ = cpstd::move(nextDenominator);
        inputHistory_ = cpstd::move(nextInputHistory);
        outputHistory_ = cpstd::move(nextOutputHistory);
        return true;
    }

    template <cpstd::size_t NumeratorCount, cpstd::size_t DenominatorCount>
    bool Configure(const CoefficientType (&numerator)[NumeratorCount],
        const CoefficientType (&denominator)[DenominatorCount]) noexcept {
        return Configure(numerator, NumeratorCount, denominator, DenominatorCount);
    }

    /** @brief Configures from any coefficient value exposing numerator and denominator arrays. */
    template <typename CoefficientSet>
    bool Configure(const CoefficientSet& coefficients) noexcept {
        return Configure(coefficients.numerator.data(), coefficients.numerator.size(),
            coefficients.denominator.data(), coefficients.denominator.size());
    }

    bool Process(const SampleType* input, SampleType* output, const cpstd::size_t sampleCount) noexcept {
        Processing::InputBlock<SampleType> inputs(input, sampleCount);
        Processing::OutputBlock<SampleType> outputs(output, sampleCount);
        return process(&inputs, 1U, &outputs, 1U) && outputs.size == sampleCount;
    }

    void Reset() noexcept { reset(); }
    cpstd::size_t NumeratorCount() const noexcept { return numerator_.empty() ? 1U : numerator_.size(); }
    cpstd::size_t DenominatorCount() const noexcept { return denominator_.empty() ? 1U : denominator_.size(); }

    cpstd::size_t inputPorts() const noexcept override { return 1U; }
    cpstd::size_t outputPorts() const noexcept override { return 1U; }

    void reset() noexcept override {
        for (cpstd::size_t index = 0U; index < inputHistory_.size(); ++index) {
            inputHistory_[index] = StateType{};
        }
        for (cpstd::size_t index = 0U; index < outputHistory_.size(); ++index) {
            outputHistory_[index] = StateType{};
        }
    }

    bool process(const Processing::InputBlock<SampleType>* inputs, const cpstd::size_t inputCount,
        Processing::OutputBlock<SampleType>* outputs, const cpstd::size_t outputCount) noexcept override {
        if (inputCount != 1U || outputCount != 1U || inputs == nullptr || outputs == nullptr ||
            !inputs[0].isValid() || !outputs[0].canWrite(inputs[0].size)) {
            return false;
        }

        for (cpstd::size_t index = 0U; index < inputs[0].size; ++index) {
            if (!DspCore::Detail::Signal::Finite(inputs[0].data[index])) {
                return false;
            }
        }
        for (cpstd::size_t index = 0U; index < inputs[0].size; ++index) {
            if (!processSample(inputs[0].data[index], outputs[0].data[index])) {
                outputs[0].size = index;
                return false;
            }
        }
        outputs[0].size = inputs[0].size;
        return true;
    }

private:
    bool processSample(const SampleType input, SampleType& output) noexcept {
        if (numerator_.empty()) {
            output = input;
            return true;
        }

        const StateType stateInput = static_cast<StateType>(input);
        if (!DspCore::Detail::Signal::Finite(stateInput)) {
            return false;
        }

        StateType sample = static_cast<StateType>(numerator_[0]) * stateInput;
        for (cpstd::size_t index = 1U; index < numerator_.size(); ++index) {
            sample += static_cast<StateType>(numerator_[index]) * inputHistory_[index - 1U];
        }
        for (cpstd::size_t index = 1U; index < denominator_.size(); ++index) {
            sample -= static_cast<StateType>(denominator_[index]) * outputHistory_[index - 1U];
        }
        if (!DspCore::Detail::Signal::Finite(sample)) {
            return false;
        }
        SampleType converted{};
        if (!DspCore::Detail::Signal::Convert(sample, converted)) {
            return false;
        }

        for (cpstd::size_t index = inputHistory_.size(); index > 1U; --index) {
            inputHistory_[index - 1U] = inputHistory_[index - 2U];
        }
        for (cpstd::size_t index = outputHistory_.size(); index > 1U; --index) {
            outputHistory_[index - 1U] = outputHistory_[index - 2U];
        }
        if (!inputHistory_.empty()) {
            inputHistory_[0] = stateInput;
        }
        if (!outputHistory_.empty()) {
            outputHistory_[0] = sample;
        }
        output = converted;
        return true;
    }

    static_assert(cpstd::is_arithmetic<SampleType>::value, "SampleType must be arithmetic");
    static_assert(cpstd::is_same<CoefficientType, float>::value || cpstd::is_same<CoefficientType, double>::value,
        "CoefficientType must be float or double");
    static_assert(cpstd::is_same<StateType, float>::value || cpstd::is_same<StateType, double>::value,
        "StateType must be float or double");

    cpstd::vector<CoefficientType> numerator_;
    cpstd::vector<CoefficientType> denominator_;
    cpstd::vector<StateType> inputHistory_;
    cpstd::vector<StateType> outputHistory_;
};

} // namespace DspCore::Filters

#endif // DSPCORE_FILTERS_IIR_H
