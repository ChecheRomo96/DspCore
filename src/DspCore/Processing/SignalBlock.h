#ifndef DSPCORE_PROCESSING_SIGNAL_BLOCK_H
#define DSPCORE_PROCESSING_SIGNAL_BLOCK_H

#include <CPtype_traits.h>

namespace DspCore::Processing {

namespace Detail {

using SignalBlockTypeKey = const void*;

template <typename SampleType>
inline constexpr unsigned char SignalBlockTypeToken = 0U;

template <typename SampleType>
constexpr SignalBlockTypeKey SignalBlockKey() noexcept {
    using ValueType = typename cpstd::remove_const<SampleType>::type;
    return &SignalBlockTypeToken<ValueType>;
}

} // namespace Detail

/**
 * @brief Non-owning common metadata for one externally owned signal-memory region.
 *
 * SignalBlockBase is only the internal heterogeneous handle used by
 * SignalManager. Applications construct SignalBlock<T> values instead. Neither
 * this class nor SignalManager deletes the samples or the block objects.
 */
class SignalBlockBase {
public:
    /** @brief Returns the number of valid samples currently contained in this block. */
    constexpr cpstd::size_t Size() const noexcept { return size_; }

    /** @brief Returns the maximum number of samples writable in this block. */
    constexpr cpstd::size_t Capacity() const noexcept { return capacity_; }

    /** @brief Returns whether this block was bound as read-only signal memory. */
    constexpr bool IsReadOnly() const noexcept { return readOnly_; }

    /** @brief Returns true when metadata and its externally supplied storage are valid. */
    constexpr bool IsValid() const noexcept {
        return size_ <= capacity_ && (capacity_ == 0U || data_ != nullptr);
    }

protected:
    constexpr SignalBlockBase(const void* data, const Detail::SignalBlockTypeKey typeKey,
        const cpstd::size_t capacity, const cpstd::size_t size, const bool readOnly) noexcept
        : data_(data), typeKey_(typeKey), capacity_(capacity), size_(size), readOnly_(readOnly) {}

private:
    friend class SignalManager;

    const void* data_{};
    Detail::SignalBlockTypeKey typeKey_{};
    cpstd::size_t capacity_{};
    cpstd::size_t size_{};
    bool readOnly_{};
};

/**
 * @brief Non-owning typed view over externally owned contiguous signal samples.
 *
 * SignalBlock never allocates, copies, or frees samples. `SignalBlock<const T>`
 * represents a read-only input region; `SignalBlock<T>` represents writable
 * output storage. A SignalManager only retains a pointer to this small view,
 * so both the view and its backing samples must outlive the process call that
 * uses them.
 *
 * `Type` exposes the C++ sample representation to typed callers. Its runtime
 * identity is intentionally private: SignalManager uses it only to validate a
 * requested `Read<T>` or `Write<T>` safely when a process has heterogeneous
 * ports.
 *
 * @tparam SampleType Sample representation; use `const T` for input blocks.
 */
template <typename SampleType>
class SignalBlock final : public SignalBlockBase {
public:
    using Type = SampleType;

    /** @brief Binds data with capacity slots, of which size already contain valid samples. */
    constexpr SignalBlock(SampleType* data = nullptr, const cpstd::size_t capacity = 0U,
        const cpstd::size_t size = 0U) noexcept
        : SignalBlockBase(data, Detail::SignalBlockKey<SampleType>(), capacity, size,
              cpstd::is_const<SampleType>::value),
          data_(data) {}

    /** @brief Returns the externally owned sample pointer with this block's access qualifier. */
    constexpr SampleType* Data() noexcept { return data_; }

    /** @brief Returns the externally owned sample pointer without granting additional write access. */
    constexpr const SampleType* Data() const noexcept { return data_; }

private:
    SampleType* data_{};
};

/** @brief Binds a caller-owned read-only input region whose full extent is valid. */
template <typename SampleType>
constexpr SignalBlock<const SampleType> MakeInputSignalBlock(const SampleType* data,
    const cpstd::size_t size) noexcept {
    return SignalBlock<const SampleType>{data, size, size};
}

/** @brief Binds caller-owned writable output storage with no produced samples yet. */
template <typename SampleType>
constexpr SignalBlock<SampleType> MakeOutputSignalBlock(SampleType* data,
    const cpstd::size_t capacity) noexcept {
    return SignalBlock<SampleType>{data, capacity, 0U};
}

} // namespace DspCore::Processing

#endif // DSPCORE_PROCESSING_SIGNAL_BLOCK_H
