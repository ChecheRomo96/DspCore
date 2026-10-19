# Generator migration

Phase 4 reviewed and reimplemented the historical generator families. Public
entry point: DspCore_Generators.h, with individual family headers under
DspCore/Generators. Core generation is independent of the LUT and
Interpolation modules; LUTSegment is available when both are enabled.

## Scope and decisions

- Periodic sine, saw, square and triangle tables retain historical phase
  conventions. They generate one cycle without a duplicate endpoint and do
  not claim band-limited synthesis or streaming oscillator functionality.
- Shaping includes raw/normalized bipolar/half atan and tanh, gamma and
  gamma morph. Strict parameter validation replaces arbitrary drive floors.
  Singleton curves sample x=0; this intentionally changes legacy gamma's
  singleton endpoint behavior. Derived gamma-morph exponents must remain
  positive and finite before any output is written.
- Linear, exponential and logarithmic segments use half-open ranges and
  exact endpoints, with singleton segments writing start. Stable local series
  replace cancellation-prone exp/log subtraction at small arguments; they
  compile even on AVR libc without expm1/log1p. Curve zero and very small
  curves use the linear limit. Oversized last indices are rejected rather
  than silently clamped.
- LUTSegment validates normalized finite shape samples, endpoints and
  disjoint storage before writing. Singleton shapes fill the whole requested
  segment with the mapped constant. This fixes the old partial-write bug.
- Noise requires explicit caller-owned nonzero uint32_t state, preserving
  sequence continuity between blocks. Zero is rejected without mutation;
  no local seed is silently restarted on each call.
- Hann/Hamming use symmetric N-1 windows, with explicit endpoint and
  singleton rules and internal constants instead of Arduino TWO_PI macros.
- All output/arithmetic is float or double. Integer PCM conversion remains
  separate work; no implicit unsafe casts into signed/unsigned integer
  sample buffers are offered. No generator allocates, changes LUT size,
  introduces a virtual Eval dependency or throws exceptions.
- Invalid descriptors/domains fail before output or PRNG state mutation.
  Scale/offset bounds and byte products are checked. Existing valid buffers
  are caller-owned; their actual extent is a caller precondition.
- Higher-dimensional LUT families are constructed through public row/slice
  storage. Tests cover 1D, 2D and 3D gamma/morph generation, replacing the
  historically uninstantiable 3D wrapper.

## Validation

- Windows MSVC: 44/44 tests pass (26 previous plus 14 generator tests and
  4 LUT-envelope/storage integration tests).
- Reference checks include phase conventions, double-precision atan/tanh,
  normalized symmetry/endpoints, gamma morph, expm1/log1p envelope values,
  zero/tiny/extreme curves, PRNG reference state and block continuity,
  Hann/Hamming coefficients, invalid byte counts and overlap rejection.
- Six native examples run with reference values and overall PASS.
- Twelve Arduino Uno sketches compile in stock C++11 mode, including all
  generator families and the LUT-envelope integration example. This is
  compile/link evidence, not physical-board execution or throughput evidence.
- Installed Release package consumer builds and exercises every family plus
  LUT-shaped envelope generation with transitive dependencies.
- MSVC AddressSanitizer probe covers all generator families at lengths 0..65,
  surrounding canaries, LUT envelopes and overlap rejection. No address
  errors reported. Artifacts are in ignored build/generators-validation.
- Doxygen generates a parent generator topic, five family topics, functions
  and examples. The existing obsolete DOT_MULTI_TARGETS warning remains.
- No Arm toolchain or hardware performance measurement was available.

No commit, tag, push or publication was performed for this phase. The next
phase is signal/system control, with its own contracts for update intervals,
reset behavior, output limits and PID anti-windup.
