#ifndef DSPCORE_PROCESSING_PROCESSOR_BASE_H
#define DSPCORE_PROCESSING_PROCESSOR_BASE_H

#include <stddef.h>

namespace DspCore { namespace Processing {

/**
 * @brief A non-owning input signal block.
 *
 * A zero-length block may use a null `data` pointer. A nonempty block
 * requires readable storage for exactly `size` values. The processor never
 * writes through this view and never retains the pointer after @ref ProcessorBase::Process
 * returns.
 */
template<typename Real> struct InputBlock {
    const Real* data;
    size_t size;

    InputBlock(const Real* values=0,size_t count=0) noexcept : data(values),size(count) {}
    bool IsValid() const noexcept { return size==0 || data!=0; }
};

/**
 * @brief A caller-owned output signal block.
 *
 * `capacity` is the writable capacity supplied by the caller. On success,
 * the processor sets `size` to the number of valid output values it wrote. A
 * processor must call `CanWrite` before writing and must not exceed
 * `capacity`. It never frees, reallocates or retains `data`.
 */
template<typename Real> struct OutputBlock {
    Real* data;
    size_t capacity;
    size_t size;

    OutputBlock(Real* values=0,size_t maximum=0) noexcept : data(values),capacity(maximum),size(0) {}
    bool CanWrite(size_t count) const noexcept { return count<=capacity && (count==0 || data!=0); }
};

/**
 * @brief MIMO base interface for a processor that consumes and produces blocks.
 *
 * A processor exposes an ordered input/output port layout. The caller
 * supplies one @ref InputBlock for each input port and one @ref OutputBlock for
 * each output port. This describes the processing contract only: it is not a
 * graph, does not connect processors, does not select an audio backend and does
 * not prescribe whether a block represents audio, modulation, measurements or
 * another signal format.
 *
 * Implementations may support blocks of one value, so sample-wise processing is
 * a valid special case. They may also require particular block sizes or produce
 * a different number of values than they consume; those requirements belong in
 * each derived class's documentation. The caller owns all supplied storage.
 * Implementations must not allocate, resize, free or retain the supplied block
 * storage during @ref Process. FIR and IIR are SISO implementations: each
 * reports one input port and one output port while retaining their direct
 * sample-processing API.
 *
 * @tparam Real Signal scalar type, normally @c float or @c double.
 */
template<typename Real> class ProcessorBase {
public:
    virtual ~ProcessorBase() {}

    /** @brief Returns the exact number of ordered input ports. */
    virtual size_t InputPorts() const noexcept=0;

    /** @brief Returns the exact number of ordered output ports. */
    virtual size_t OutputPorts() const noexcept=0;

    /**
     * @brief Processes one block for every input and output port.
     *
     * @p inputs and @p outputs must be non-null when their corresponding count
     * is nonzero. @p inputCount must equal @ref InputPorts and @p outputCount
     * must equal @ref OutputPorts. Each input must be valid and each output
     * must have enough capacity for the values the implementation intends to
     * write. On success, every output's `size` is the number of
     * valid values written. On failure, the implementation returns @c false,
     * preserves its persistent state and leaves every output block and its
     * `size` unchanged. Input and output storage may overlap
     * only when the derived processor explicitly documents support for it.
     */
    virtual bool Process(const InputBlock<Real>* inputs,size_t inputCount,
                         OutputBlock<Real>* outputs,size_t outputCount) noexcept=0;

    /**
     * @brief Restores the processor to its documented initial signal state.
     *
     * Reset does not change the port count, configuration or caller-owned
     * buffers. It is not safe to call concurrently with @ref Process unless a
     * derived processor explicitly documents synchronization.
     */
    virtual void Reset() noexcept=0;
};

}}

#endif
