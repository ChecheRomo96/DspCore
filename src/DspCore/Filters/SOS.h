#ifndef DSPCORE_FILTERS_SOS_H
#define DSPCORE_FILTERS_SOS_H

/**
 * @file SOS.h
 * @ingroup DspCore_Filters
 * @brief Reserved home for cascaded second-order-section filter execution.
 *
 * @par Status
 * Planned; this header intentionally declares no processor yet.
 *
 * A future SOS processor will execute a sequence of second-order IIR sections
 * with explicit per-section state. `Biquad` already designs one section's
 * coefficients; a cascade must additionally specify section ordering,
 * coefficient scaling, state precision, and failure behavior. Those choices
 * require a dedicated API and tests rather than an empty placeholder class.
 */

#endif // DSPCORE_FILTERS_SOS_H
