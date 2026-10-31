#ifndef DSPCORE_DYNAMICS_COMPRESSOR_H
#define DSPCORE_DYNAMICS_COMPRESSOR_H

/**
 * @file Compressor.h
 * @ingroup DspCore_Dynamics
 * @brief Reserved home for a stateful dynamic-range compressor.
 *
 * @par Status
 * Planned; no compressor API is declared yet.
 *
 * A compressor needs an explicit detector, gain computer, smoothing law,
 * makeup gain, look-ahead policy, and channel-link rule. It is not LTI: its
 * gain changes with signal level. The eventual processor will retain its own
 * detector state while accepting caller-owned blocks.
 */

#endif // DSPCORE_DYNAMICS_COMPRESSOR_H
