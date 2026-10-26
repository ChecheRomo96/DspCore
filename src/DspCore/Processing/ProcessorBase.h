#ifndef DSPCORE_PROCESSING_PROCESSOR_BASE_H
#define DSPCORE_PROCESSING_PROCESSOR_BASE_H

#include <CPutility.h>

namespace DspCore::Processing {

/** @brief Non-owning immutable signal block supplied by a caller. */
template <typename SampleType>
struct InputBlock {
    const SampleType* data{};
    cpstd::size_t size{};

    constexpr InputBlock(const SampleType* values = nullptr, const cpstd::size_t count = 0U) noexcept
        : data(values), size(count) {}

    constexpr bool isValid() const noexcept { return size == 0U || data != nullptr; }
};

/** @brief Caller-owned writable signal block. */
template <typename SampleType>
struct OutputBlock {
    SampleType* data{};
    cpstd::size_t capacity{};
    cpstd::size_t size{};

    constexpr OutputBlock(SampleType* values = nullptr, const cpstd::size_t maximum = 0U) noexcept
        : data(values), capacity(maximum) {}

    constexpr bool canWrite(const cpstd::size_t count) const noexcept {
        return count <= capacity && (count == 0U || data != nullptr);
    }
};

/**
 * @brief Common contract for a processor that transforms one block of signal samples into another.
 *
 * A processor is modeled as a node with a fixed number of input and output
 * signal ports. A port is one independent sample stream, represented for one
 * call by an InputBlock or OutputBlock; it is not an audio-channel count.
 * The caller owns all block memory and provides the port arrays to process.
 *
 * Implementations may keep time-dependent state between calls. reset removes
 * that state while retaining the current processor configuration, allowing a
 * stream to start again with defined initial conditions.
 */
template <typename SampleType>
class ProcessorBase {
public:
    virtual ~ProcessorBase() = default;

    /**
     * @brief Returns how many input signal streams this processor requires.
     *
     * A successful process call receives exactly this many InputBlock values.
     * The count describes the processor topology; it does not describe how
     * many samples are in a block.
     */
    virtual cpstd::size_t inputPorts() const noexcept = 0;

    /**
     * @brief Returns how many output signal streams this processor produces.
     *
     * A successful process call receives exactly this many OutputBlock values.
     * Each output block reports how many samples were produced through its
     * size member.
     */
    virtual cpstd::size_t outputPorts() const noexcept = 0;

    /**
     * @brief Processes one caller-owned block for every input and output port.
     *
     * inputs and outputs point to arrays whose counts must match inputPorts()
     * and outputPorts(). Each InputBlock identifies immutable source samples;
     * each OutputBlock identifies writable destination storage and its
     * capacity. On success, the implementation updates every output size to
     * the number of samples it produced.
     *
     * @return true when the supplied port topology and blocks were accepted
     * and processed; otherwise false.
     */
    virtual bool process(const InputBlock<SampleType>* inputs, cpstd::size_t inputCount,
        OutputBlock<SampleType>* outputs, cpstd::size_t outputCount) noexcept = 0;

    /**
     * @brief Removes time-dependent processing state without changing the configured design.
     *
     * After reset, the next process call behaves as if the processor had just
     * been configured with its current parameters: delay lines, integrators,
     * and other runtime memory begin from their defined initial condition.
     */
    virtual void reset() noexcept = 0;
};

} // namespace DspCore::Processing

#endif // DSPCORE_PROCESSING_PROCESSOR_BASE_H
