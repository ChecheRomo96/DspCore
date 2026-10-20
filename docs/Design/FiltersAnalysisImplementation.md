# Streaming filters and analysis

Phase 5 adds generic DspCore::Filters processors and DspCore::Analysis.
The user refined the architecture: only FIR and IIR process signal blocks;
Biquad groups the free coefficient calculators and the returned struct.
Control systems, including Smoother, are outside DspCore.

## Filter architecture

- FIR<T> owns growable numerator/history cpstd::vector storage. Configure
  accepts a positive runtime count, validates that many finite taps, resizes
  outside Process and resets history. Default is one-tap identity.
- IIR<T> owns growable numerator, denominator and history cpstd::vector storage.
  It accepts independently sized positive raw arrays/counts, array references or a
  struct exposing those arrays. a0 must be finite/nonzero. All coefficients are
  normalized by a0 and checked before state is committed; Configure may resize and
  reset histories while Process never allocates.
- The generic direct-form I recurrence handles arbitrary configured counts,
  independent of any calculator. Successful Configure resets histories;
  invalid normalization/configuration or sample arithmetic preserves all state
  and output. There is no automatic arbitrary-order pole calculation.
- Namespace Filters::Biquad contains Coefficients<T> with numerator[3] and
  denominator[3], LowPass/HighPass functions returning it by value, and IsStable.
  No Biquad processing class, root BiquadCoefficients type, Design namespace or
  bool/out-parameter coefficient builders remain in the source API.
- Calculators retain raw a0 in the returned six-scalar struct. Invalid designs
  return denominator[0]==0 so IIR::Configure rejects them. Calculators validate
  strict second-order stability; user-supplied generic IIR stability belongs to
  the caller. Coefficients can be shared as values across independent filters.
- Low/high-pass calculations follow the W3C Audio EQ Cookbook's RBJ equations,
  with half-angle numerator identities. Near-marginal designs that cannot be
  represented stably in the chosen precision are rejected.
- Both processors own their dynamic state through CPSTL vectors. Size budgets cover
  their processor objects and six-coefficient values. Coefficient changes may cause
  transients; no automatic interpolation or clipping is introduced.

## Analysis

Peak and RMS measure only their supplied block. Empty blocks produce zero;
invalid buffers/non-finite samples preserve output. RMS scales by peak before
squaring to avoid overflow and bounds normalized roundoff before rescaling.
There is no implicit DC removal, sliding window or dB conversion. Output may
alias an input element because all reads finish before output is committed.
EnvelopeFollower rectifies samples and applies explicit attack/release seconds
with stable one-pole coefficients. Zero time constants snap; invalid updates
preserve state. No clock, scheduler or MIDI interpretation is introduced.

## Verification

- Windows MSVC: 63/63 tests pass (44 previous, 12 filter/calculator, 7 analysis).
- Filter tests cover FIR tap order, resets, generic IIR 4/2 and 1/1 counts,
  nonunit a0 normalization, independent difference-equation references,
  six returned coefficients, calculator validation, pole boundaries, separate
  filter instances and transactional overflow/configuration failures.
- Frequency checks cover known quarter-rate Butterworth coefficients,
  DC/Nyquist rejection/pass response and cutoff power.
- Analysis checks include block/sine/full-scale/small-signal RMS, aliasing,
  empty/invalid input, attack/release references and double precision.
- Five native examples pass, including the new generic Filters/IIR example
  and Filters/Biquad configuring an IIR from a returned coefficient value.
- Seventeen Arduino Uno sketches compile in stock C++11 (compile/link only;
  no board execution or throughput measurement).
- Installed Release consumer builds/runs using the new calculator/IIR API.
- AddressSanitizer passed 10,000 FIR/IIR/follower updates plus block lengths
  0..65 with surrounding canaries. Probe source/binary are under ignored
  build/filters-validation; no address errors were reported.
- Doxygen exposes FIR/IIR processing and a nested Biquad calculator topic.
  The local obsolete DOT_MULTI_TARGETS warning remains with Doxygen 1.18.
- No physical-board performance or Arm validation was performed.

Design reference: https://www.w3.org/TR/audio-eq-cookbook/ . Equations are
implemented directly; no third-party implementation code is imported.
Next phase remains transforms, subject to the user's next instruction.
