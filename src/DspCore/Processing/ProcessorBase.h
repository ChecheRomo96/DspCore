#ifndef DSPCORE_PROCESSING_PROCESSOR_BASE_H
#define DSPCORE_PROCESSING_PROCESSOR_BASE_H

#include <CPutility.h>

namespace DspCore::Processing {

/** @brief Opaque identity for the C++ type carried by one signal port. */
using SampleTypeId = const void*;

/** @brief Creates one program-wide type token for every SampleType. */
template <typename SampleType>
inline constexpr unsigned char SampleTypeToken = 0U;

/** @brief Returns the implicit identity associated with SampleType. */
template <typename SampleType>
constexpr SampleTypeId SampleTypeIdentifier() noexcept {
    return &SampleTypeToken<SampleType>;
}

/** @brief Non-owning immutable samples presented at one MIMO input port. */
struct InputBuffer {
    const void* data{};
    SampleTypeId sampleType{};
    cpstd::size_t size{};

    constexpr bool isValid() const noexcept {
        return sampleType != nullptr && (size == 0U || data != nullptr);
    }
};

/** @brief Caller-owned writable samples presented at one MIMO output port. */
struct OutputBuffer {
    void* data{};
    SampleTypeId sampleType{};
    cpstd::size_t capacity{};
    cpstd::size_t size{};

    constexpr bool canWrite(const cpstd::size_t count) const noexcept {
        return sampleType != nullptr && count <= capacity && (count == 0U || data != nullptr);
    }
};

/** @brief Creates a typed input-port descriptor without transferring ownership. */
template <typename SampleType>
constexpr InputBuffer MakeInputBuffer(const SampleType* data, const cpstd::size_t size) noexcept {
    return InputBuffer{data, SampleTypeIdentifier<SampleType>(), size};
}

/** @brief Creates a typed output-port descriptor without transferring ownership. */
template <typename SampleType>
constexpr OutputBuffer MakeOutputBuffer(SampleType* data, const cpstd::size_t capacity) noexcept {
    return OutputBuffer{data, SampleTypeIdentifier<SampleType>(), capacity, 0U};
}

/**
 * @brief Type-erased multiple-input, multiple-output signal-processing contract.
 *
 * Each port carries an InputBuffer or OutputBuffer with its own SampleTypeId,
 * size, and storage. This permits heterogeneous graphs: a processor may accept
 * an integer acquisition stream and a floating-point control stream, then
 * produce a different output representation. The caller owns every buffer and
 * passes arrays of pointers because the ports are independently described.
 *
 * Implementations may keep time-dependent state between calls. reset removes
 * that state while retaining the current processor configuration, allowing a
 * stream to start again with defined initial conditions.
 */
class ProcessorBase {
public:
    virtual ~ProcessorBase() = default;

    /**
     * @brief Returns how many input signal streams this processor requires.
     *
     * A successful process call receives exactly this many InputBuffer values.
     * The count describes the processor topology; it does not describe how
     * many samples are in a block.
     */
    virtual cpstd::size_t inputPorts() const noexcept = 0;

    /**
     * @brief Returns the required C++ sample type for an input port.
     * @return nullptr when port is outside [0, inputPorts()).
     */
    virtual SampleTypeId inputSampleType(cpstd::size_t port) const noexcept = 0;

    /**
     * @brief Returns how many output signal streams this processor produces.
     *
     * A successful process call receives exactly this many OutputBuffer values.
     * Each output block reports how many samples were produced through its
     * size member.
     */
    virtual cpstd::size_t outputPorts() const noexcept = 0;

    /**
     * @brief Returns the produced C++ sample type for an output port.
     * @return nullptr when port is outside [0, outputPorts()).
     */
    virtual SampleTypeId outputSampleType(cpstd::size_t port) const noexcept = 0;

    /**
     * @brief Processes one caller-owned block for every input and output port.
     *
     * inputBuffers and outputBuffers point to arrays whose counts must match
     * inputPorts() and outputPorts(). Their per-port types must match
     * inputSampleType() and outputSampleType(). Each input size may differ; each output
     * capacity bounds what may be written. On success, the implementation sets
     * every output size to the number of samples it produced.
     *
     * @return true when the supplied port topology and blocks were accepted
     * and processed; otherwise false.
     */
    virtual bool process(const InputBuffer* const* inputBuffers, cpstd::size_t inputCount,
        OutputBuffer* const* outputBuffers, cpstd::size_t outputCount) noexcept = 0;

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
