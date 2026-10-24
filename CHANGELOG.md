# Changelog

This file records user-visible changes to DspCore. Release dates use the
`YYYY-MM-DD` format.

## [Unreleased]

### Changed

- DspCore requires Foundation 2.0.10 or a newer 2.x release, which builds on
  CPSTL 1.1.12. Arduino users install CPSTL next to Foundation;
  `scripts/test-arduino.sh` and `.ps1` take `--cpstl` (default
  `DSPCORE_CPSTL_SOURCE` or `../CPSTL`).

- Arduino sketches no longer need to include `<Foundation.h>`: every DspCore
  header brings in Foundation, so including `<DspCore.h>` or a single module
  header such as `<DspCore_Core.h>` is enough for the Arduino builder to find
  both libraries. The example includes only the module it demonstrates.

### Added

- `DspCore::Filters::FIR<SampleType, CoefficientType, StateType>` and
  `IIR<SampleType, CoefficientType, StateType>`, runtime-order SISO processors
  with independent stream, coefficient and recurrence-state representations.
  `CoefficientType` defaults to `float`; `StateType` defaults to
  `CoefficientType`.
- `DspCore::Filters::Biquad` coefficient designers for RBJ low-pass, high-pass,
  band-pass, notch, all-pass, peaking-EQ, low-shelf and high-shelf designs.
  Every designer returns a coefficient value ready for `IIR::Configure`.
  `Biquad::Design<CoefficientType>` chooses the design precision explicitly
  and defaults it to `float`.
- `Biquad::Parameters(Biquad::Type)` exposes allocation-free metadata for the
  controls required by each design, including UI labels and units.
- 0.1.0 scaffold: RoModularBuild, native, AVR and Arm presets, Bash and
  PowerShell scripts, Foundation `1.4.0` dependency resolution, installable
  CMake package with a consumer test, Arduino library layout, Doxygen
  documentation and CI (native, embedded, Arduino, documentation, sanitizers
  and clang-tidy).
- `DspCore::Core::Version()` and `FoundationVersion()`.
