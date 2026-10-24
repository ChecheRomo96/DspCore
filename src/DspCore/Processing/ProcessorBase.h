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

/** @brief Abstract block-processing interface for signal processors. */
template <typename SampleType>
class ProcessorBase {
public:
    virtual ~ProcessorBase() = default;

    virtual cpstd::size_t inputPorts() const noexcept = 0;
    virtual cpstd::size_t outputPorts() const noexcept = 0;
    virtual bool process(const InputBlock<SampleType>* inputs, cpstd::size_t inputCount,
        OutputBlock<SampleType>* outputs, cpstd::size_t outputCount) noexcept = 0;
    virtual void reset() noexcept = 0;
};

} // namespace DspCore::Processing

#endif // DSPCORE_PROCESSING_PROCESSOR_BASE_H
