#ifndef DSPCORE_PROCESSING_PROCESSOR_BASE_H
#define DSPCORE_PROCESSING_PROCESSOR_BASE_H

#include <DspCore/Processing/SignalManager.h>

namespace DspCore::Processing {

/**
 * @brief Type-erased multiple-input, multiple-output signal-processing contract.
 *
 * SignalManager supplies ordered non-owning input and output SignalBlock
 * bindings. This permits heterogeneous graphs: a processor may accept an
 * integer acquisition block and a floating-point control block, then produce a
 * different output representation. The host owns all signal memory; a
 * processor owns only algorithm-specific state such as delay lines.
 *
 * Implementations may keep time-dependent state between calls. reset removes
 * that state while retaining the current processor configuration, allowing a
 * stream to start again with defined initial conditions.
 */
class ProcessorBase {
public:
    virtual ~ProcessorBase() = default;

    /**
     * @brief Transforms ordered externally owned input blocks into output blocks.
     *
     * A concrete processor declares its port order with named enums, validates
     * its expected Count values, and uses SignalManager::Read<T> or Write<T>.
     * It calls outputs.Commit after filling an output. The call does not imply
     * a stream clock, FIFO, allocation, or ownership transfer.
     *
     * @return true when the supplied port topology and blocks were accepted
     * and processed; otherwise false.
     */
    virtual bool process(const SignalManager& inputs, SignalManager& outputs) noexcept = 0;

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
