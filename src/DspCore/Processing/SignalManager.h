#ifndef DSPCORE_PROCESSING_SIGNAL_MANAGER_H
#define DSPCORE_PROCESSING_SIGNAL_MANAGER_H

#include <DspCore/Processing/SignalBlock.h>

namespace DspCore::Processing {

/**
 * @brief Ordered, non-owning collection of heterogeneous SignalBlock bindings.
 *
 * A manager carries the signals for one side of a ProcessorBase invocation.
 * It does not own samples, blocks, clocks, queues, or graph connections. A
 * host such as JUCE, an embedded callback, or a future SignalGraph owns that
 * state, binds the appropriate blocks in port order, then calls process.
 *
 * `Read<T>(index)` accepts only a SignalBlock<const T>; `Write<T>(index)`
 * accepts only a SignalBlock<T>. Both validate the requested type internally.
 * The implementation detail that permits heterogeneous collections is not
 * exposed in the public API and does not restrict user-defined sample types.
 *
 * Output blocks begin with Size()==0. A processor calls Commit after writing to
 * record its produced extent. A failed read, write, or commit leaves every
 * bound block unchanged.
 */
class SignalManager {
public:
    /** @brief Binds blocks in the processor-defined port order without taking ownership. */
    constexpr SignalManager(SignalBlockBase* const* blocks = nullptr, const cpstd::size_t count = 0U) noexcept
        : blocks_(blocks), count_(count) {}

    /** @brief Returns the number of ordered signal bindings. */
    constexpr cpstd::size_t Count() const noexcept { return count_; }

    /** @brief Returns true when every bound block is present and valid. */
    bool IsValid() const noexcept {
        if (count_ != 0U && blocks_ == nullptr) {
            return false;
        }
        for (cpstd::size_t index = 0U; index < count_; ++index) {
            if (blocks_[index] == nullptr || !blocks_[index]->IsValid()) {
                return false;
            }
        }
        return true;
    }

    /** @brief Returns read-only block index as T, or nullptr for an invalid index or type. */
    template <typename SampleType>
    const SignalBlock<const SampleType>* Read(const cpstd::size_t index) const noexcept {
        static_assert(!cpstd::is_const<SampleType>::value, "Read expects an unqualified sample type");
        const SignalBlockBase* block = BlockAt(index);
        if (block == nullptr || !block->readOnly_ ||
            block->typeKey_ != Detail::SignalBlockKey<SampleType>()) {
            return nullptr;
        }
        return static_cast<const SignalBlock<const SampleType>*>(block);
    }

    /** @brief Returns writable block index as T, or nullptr for an invalid index or type. */
    template <typename SampleType>
    SignalBlock<SampleType>* Write(const cpstd::size_t index) noexcept {
        static_assert(!cpstd::is_const<SampleType>::value, "Write expects an unqualified sample type");
        SignalBlockBase* block = BlockAt(index);
        if (block == nullptr || block->readOnly_ ||
            block->typeKey_ != Detail::SignalBlockKey<SampleType>()) {
            return nullptr;
        }
        return static_cast<SignalBlock<SampleType>*>(block);
    }

    /**
     * @brief Records count produced samples for a writable output block.
     * @return false when index is invalid, bound to an input block, or count
     *         exceeds capacity. The previous output size is retained on failure.
     */
    bool Commit(const cpstd::size_t index, const cpstd::size_t count) noexcept {
        SignalBlockBase* block = BlockAt(index);
        if (block == nullptr || block->readOnly_ || count > block->capacity_) {
            return false;
        }
        block->size_ = count;
        return true;
    }

private:
    SignalBlockBase* BlockAt(const cpstd::size_t index) noexcept {
        return index < count_ && blocks_ != nullptr ? blocks_[index] : nullptr;
    }

    const SignalBlockBase* BlockAt(const cpstd::size_t index) const noexcept {
        return index < count_ && blocks_ != nullptr ? blocks_[index] : nullptr;
    }

    SignalBlockBase* const* blocks_{};
    cpstd::size_t count_{};
};

} // namespace DspCore::Processing

#endif // DSPCORE_PROCESSING_SIGNAL_MANAGER_H
