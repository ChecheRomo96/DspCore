#ifndef DSPCORE_FILTERS_FIR_H
#define DSPCORE_FILTERS_FIR_H

#include <CPutility.h>
#include <CPvector.h>
#include <DspCore/Detail/SignalMath.h>
#include <DspCore/Processing/ProcessorBase.h>

namespace DspCore::Filters {

/**
 * @brief Runtime-tap finite impulse response SISO processor.
 *
 * SampleType is the stream representation. CoefficientType controls the tap
 * representation, while StateType controls the multiply-accumulate and delay
 * line precision.
 */
template <typename SampleType = float, typename CoefficientType = float, typename StateType = CoefficientType>
class FIR final : public Processing::ProcessorBase<SampleType> {
public:
    /** @brief Owns a runtime-length FIR tap configuration outside the audio path. */
    struct Coefficients {
        cpstd::vector<CoefficientType> taps;

        bool Assign(const CoefficientType* values, const cpstd::size_t count) noexcept {
            if (values == nullptr || count == 0U) {
                return false;
            }
            cpstd::vector<CoefficientType> next;
            next.resize(count);
            if (next.size() != count) {
                return false;
            }
            for (cpstd::size_t index = 0U; index < count; ++index) {
                if (!DspCore::Detail::Signal::Finite(values[index])) {
                    return false;
                }
                next[index] = values[index];
            }
            taps = cpstd::move(next);
            return true;
        }
    };

    /** @brief Sets `b[0]...b[N-1]`; successful configuration clears delay state. */
    bool Configure(const CoefficientType* coefficients, const cpstd::size_t count) noexcept {
        if (coefficients == nullptr || count == 0U) {
            return false;
        }

        cpstd::vector<CoefficientType> nextCoefficients;
        cpstd::vector<StateType> nextHistory;
        nextCoefficients.resize(count);
        nextHistory.resize(count);
        if (nextCoefficients.size() != count || nextHistory.size() != count) {
            return false;
        }

        for (cpstd::size_t index = 0U; index < count; ++index) {
            if (!DspCore::Detail::Signal::Finite(coefficients[index])) {
                return false;
            }
            nextCoefficients[index] = coefficients[index];
        }

        coefficients_ = cpstd::move(nextCoefficients);
        history_ = cpstd::move(nextHistory);
        next_ = 0U;
        return true;
    }

    template <cpstd::size_t Count>
    bool Configure(const CoefficientType (&coefficients)[Count]) noexcept {
        return Configure(coefficients, Count);
    }

    bool Configure(const Coefficients& coefficients) noexcept {
        return Configure(coefficients.taps.data(), coefficients.taps.size());
    }

    bool Process(const SampleType* input, SampleType* output, const cpstd::size_t sampleCount) noexcept {
        Processing::InputBlock<SampleType> inputs(input, sampleCount);
        Processing::OutputBlock<SampleType> outputs(output, sampleCount);
        return process(&inputs, 1U, &outputs, 1U) && outputs.size == sampleCount;
    }

    void Reset() noexcept { reset(); }
    cpstd::size_t Taps() const noexcept { return coefficients_.empty() ? 1U : coefficients_.size(); }

    cpstd::size_t inputPorts() const noexcept override { return 1U; }
    cpstd::size_t outputPorts() const noexcept override { return 1U; }

    void reset() noexcept override {
        for (cpstd::size_t index = 0U; index < history_.size(); ++index) {
            history_[index] = StateType{};
        }
        next_ = 0U;
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
        if (coefficients_.empty()) {
            output = input;
            return true;
        }

        const StateType stateInput = static_cast<StateType>(input);
        if (!DspCore::Detail::Signal::Finite(stateInput)) {
            return false;
        }

        StateType sum = static_cast<StateType>(coefficients_[0]) * stateInput;
        cpstd::size_t historyIndex = next_;
        for (cpstd::size_t tap = 1U; tap < coefficients_.size(); ++tap) {
            historyIndex = historyIndex == 0U ? coefficients_.size() - 1U : historyIndex - 1U;
            sum += static_cast<StateType>(coefficients_[tap]) * history_[historyIndex];
        }
        if (!DspCore::Detail::Signal::Finite(sum)) {
            return false;
        }
        SampleType converted{};
        if (!DspCore::Detail::Signal::Convert(sum, converted)) {
            return false;
        }

        history_[next_] = stateInput;
        next_ = next_ == coefficients_.size() - 1U ? 0U : next_ + 1U;
        output = converted;
        return true;
    }

    static_assert(cpstd::is_arithmetic<SampleType>::value, "SampleType must be arithmetic");
    static_assert(cpstd::is_same<CoefficientType, float>::value || cpstd::is_same<CoefficientType, double>::value,
        "CoefficientType must be float or double");
    static_assert(cpstd::is_same<StateType, float>::value || cpstd::is_same<StateType, double>::value,
        "StateType must be float or double");

    cpstd::vector<CoefficientType> coefficients_;
    cpstd::vector<StateType> history_;
    cpstd::size_t next_{};
};

} // namespace DspCore::Filters

#endif // DSPCORE_FILTERS_FIR_H
