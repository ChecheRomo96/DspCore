#ifndef DSPCORE_FILTERS_MULTIRATE_H
#define DSPCORE_FILTERS_MULTIRATE_H

/**
 * @file Multirate.h
 * @ingroup DspCore_Filters
 * @brief Reserved home for decimation, interpolation, and rational rate conversion.
 *
 * @par Status
 * Planned; this header intentionally declares no rate-conversion processor yet.
 *
 * A multirate filter combines an anti-alias or anti-imaging filter with a
 * sample-rate change. It must make the input/output rate ratio, phase, delay,
 * block-size relation, and output-capacity contract explicit. Audio-device
 * clocks, queues, and stream ownership remain outside DspCore.
 */

#endif // DSPCORE_FILTERS_MULTIRATE_H
