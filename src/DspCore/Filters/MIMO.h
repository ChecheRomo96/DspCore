#ifndef DSPCORE_FILTERS_MIMO_H
#define DSPCORE_FILTERS_MIMO_H

/**
 * @file MIMO.h
 * @ingroup DspCore_Filters
 * @brief Reserved home for multiple-input multiple-output linear filters.
 *
 * @par Status
 * Planned; this header intentionally declares no MIMO processor yet.
 *
 * A MIMO FIR has a matrix of taps for every delay. A MIMO recursive filter is
 * usually clearer as a state-space realization with matrices @f$A,B,C,D@f$.
 * That future API will define channel/frame layout and fixed-size or
 * caller-supplied matrix storage. It will not introduce stream routing,
 * heterogeneous ports, FIFOs, or scheduling into DspCore.
 */

#endif // DSPCORE_FILTERS_MIMO_H
