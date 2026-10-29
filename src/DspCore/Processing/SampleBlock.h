#ifndef DSPCORE_PROCESSING_SAMPLE_BLOCK_H
#define DSPCORE_PROCESSING_SAMPLE_BLOCK_H

#include <CPtype_traits.h>

namespace DspCore::Processing {

/**
 * @brief Non-owning typed view of a fixed-size contiguous sample region.
 *
 * SampleBlock only describes caller-owned memory; it never allocates, copies,
 * frees, queues, schedules, or resamples samples. `SampleBlock<const T>` is a
 * read-only input view and `SampleBlock<T>` is writable output storage. The
 * same template therefore expresses the access contract directly in C++.
 *
 * Its `size` is the complete extent of the view. DspCore's n-to-n algorithms
 * require their input and output blocks to have equal sizes, so there is no
 * separate capacity, produced-count, commit operation, or hidden buffering.
 * The caller keeps both the view and the backing samples alive for the whole
 * Process call.
 *
 * @tparam SampleType Element type. Use `const T` to create an input block.
 */
template <typename SampleType>
struct SampleBlock {
    using Type = SampleType;

    SampleType* data{};
    cpstd::size_t size{};

    /** @brief Binds a caller-owned contiguous region without taking ownership. */
    constexpr SampleBlock(SampleType* values = nullptr, const cpstd::size_t count = 0U) noexcept
        : data(values), size(count) {}

    /** @brief Returns true when a non-empty view has a valid data pointer. */
    constexpr bool IsValid() const noexcept { return size == 0U || data != nullptr; }
};

/**
 * @brief Creates a read-only view over caller-owned input samples.
 *
 * This is a convenience adapter for APIs that expose a pointer and sample
 * count. When the caller already manages SampleBlock instances, construct and
 * retain `SampleBlock<const T>` directly instead.
 */
template <typename SampleType>
constexpr SampleBlock<const SampleType> MakeInputBlock(const SampleType* data,
    const cpstd::size_t size) noexcept {
    return SampleBlock<const SampleType>{data, size};
}

/**
 * @brief Creates a writable view over caller-owned output samples.
 *
 * This is a convenience adapter for APIs that expose a pointer and sample
 * count. When the caller already manages SampleBlock instances, construct and
 * retain `SampleBlock<T>` directly instead.
 */
template <typename SampleType>
constexpr SampleBlock<SampleType> MakeOutputBlock(SampleType* data,
    const cpstd::size_t size) noexcept {
    return SampleBlock<SampleType>{data, size};
}

} // namespace DspCore::Processing

#endif // DSPCORE_PROCESSING_SAMPLE_BLOCK_H
