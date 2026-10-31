# Interpolation migration

Phase 3 implements `DspCore::Interpolation` independently of generators.
Historical source was reviewed rather than copied verbatim.

## API and numerical decisions

- Arithmetic.h provides NormalizedLinear, NormalizedBilinear and
  NormalizedTrilinear with explicitly consistent float or double
  samples/coordinates. LinearAt, BilinearAt and TrilinearAt normalize physical
  coordinates before applying those formulas. No float-only return type,
  integer arithmetic, allocation, hidden clipping or implicit edge policy.
- Normalized formulas extrapolate. NormalizedLinear selects exact endpoints and avoids
  subtracting opposite-sign finite endpoints during interpolation.
- Corner order follows x-fast LUT storage; asymmetric field tests distinguish
  x, y and z rather than testing only a symmetric center average.
- Physical coordinate endpoints use 0/1 names rather than min/max: descending
  axes are valid. A collapsed physical axis maps to normalized coordinate zero
  and selects its 0-side, avoiding division by zero. The interpolation API is
  mathematical and returns a value directly; it does not expose Try variants
  or validation booleans.
- Lookup overloads for 1D/2D/3D require an explicit Boundary policy. Clamp and
  Reject use normalized closed coordinates with N-1 intervals. Wrap uses N
  distinct periodic samples with the last-to-first interval included. All
  axes share the chosen policy; per-axis policies remain a future extension.
- Singleton axes are constant but still validate coordinates and Reject.
  Empty tables and invalid/non-finite coordinates fail without writing output.
- Floating-to-index conversion is guarded by bounds and exact-integer extent
  checks. Sample type conversion is independent: integer values are not PCM
  normalized, double computation stays double, and narrowing range overflow
  explicitly produces signed infinity instead of an out-of-range cast.
- Boundary uses a one-byte enum with target-build size/triviality assertions.
- Arithmetic is available without LUT; Lookup is staged/installed when the
  LUT module is enabled. Arduino C++11 can discover the module facade.

## Validation

- Windows MSVC: 26/26 tests pass (12 previous, 6 arithmetic, 8 lookup).
- Tests include double precision, endpoint selection, opposite-sign maximum
  samples, extrapolation, reversed and collapsed physical axes, affine planes/volumes,
  negative cycles, periodic seams, singleton axes, NaN/Inf coordinates,
  adjacent floating endpoints and explicit sample narrowing overflow.
- Both new native examples run with actual values/expectations and PASS.
- All six Arduino Uno examples compile under stock C++11; no board execution
  or throughput measurement was performed. Reported static memory excludes
  dynamically allocated LUT capacity.
- Installed Release package consumer builds/runs interpolation and LUT lookup.
- AddressSanitizer standalone probe passed 18,009 lookups across three ranks
  and boundary policies plus extreme finite coordinates. Probe artifacts are
  under ignored build/interpolation-validation; no address errors reported.
- Doxygen generates the linked topic, functions and Boundary enum. The
  existing obsolete DOT_MULTI_TARGETS configuration warning remains.
- Arm/hardware validation remains unavailable in this environment.

The native tests, examples, package and documentation were exercised through
supported scripts. No commit, push, tag or publication was performed.
