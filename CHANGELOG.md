# Changelog

This file records user-visible changes to DspCore. Release dates use the
`YYYY-MM-DD` format.

## Unreleased

### Added

- `DspCore::Processing::ProcessorBase`, a MIMO virtual block-processing
  contract with non-owning input/output views and caller-controlled storage.
- FIR and IIR implement `ProcessorBase` as SISO nodes while retaining their
  direct `Process(sample, output)` APIs. Their CPSTL vector-backed coefficients
  and histories now resize at runtime through Configure.

### Changed

- FIR/IIR processor objects are polymorphic and retain explicit size budgets;
  they are no longer trivially-copyable value types.

## [0.2.0] - 2026-10-19

### Fixed

- Include ABI alignment padding in FIR state size budgets on ARM targets.

### Removed

- Control module, including Smoother, and its examples, tests and documentation;
  control systems are outside DspCore.

### Added

- Generic FIR/IIR processors and Biquad free functions returning six-coefficient
  structs by value; IIR normalizes a0 during configuration. Low/high-pass coefficient
  design, scaled block RMS/peak and attack/release envelope following.
- Filter/analysis reference tests, five portable examples and module topics.
- Allocation-free float/double periodic tables, shaping curves, stable envelope
  segments, explicit-state white noise and symmetric Hann/Hamming windows.
- LUT-shaped envelopes with checked overlap and complete singleton-shape filling.
- Generator reference/regression tests, six Arduino/desktop examples and family topics.
- Float/double linear, bilinear and trilinear interpolation, checked physical
  coordinates and explicit Clamp/Wrap/Reject LUT lookup.
- Interpolation and boundary regression tests, two Arduino/desktop examples
  and an API topic documenting corner order, periodic seams and precision.
- Move-only LUT1D/2D/3D storage with checked sizes, zero-initialized growth,
  reserve, checked access and explicit release.
- LUT regression/failure tests and three Arduino/desktop examples.
- Numeric/buffer specification and signal-processing architecture.
- Compile-time float/double precision requirements for every library target.
- Legacy DSP review documenting defects to resolve during migration.

## [0.1.3] - 2026-10-16

### Changed

- Remove the redundant library-name directory from examples; update CMake, Arduino validation and documentation.
- Require Foundation 2.0.8 and validate against CPSTL 1.1.12 in embedded CI.

## [0.1.2] - 2026-10-16

### Changed

- Pin RoModularBuild v0.6.2 and consume its shared compiler/preset interfaces.
- Add ATmega2560 / AVR6 and STM32G0B1 platform coverage to embedded builds.
- Refresh released dependency pins across CMake and Arduino CI.

## [0.1.1] - 2026-10-08

### Changed

- Arduino source builds now accept stock C++11 cores without injecting
  `-std=gnu++17`. CMake and direct-source integrations remain C++17.

## [0.1.0] - 2026-10-08

### Changed

- DspCore requires Foundation 2.0.5 or a newer 2.x release, which builds on
  CPSTL 1.1.5. Arduino users install CPSTL next to Foundation;
  `scripts/test-arduino.sh` and `.ps1` take `--cpstl` (default
  `DSPCORE_CPSTL_SOURCE` or `../CPSTL`).

- Arduino sketches no longer need to include `<Foundation.h>`: every DspCore
  header brings in Foundation, so including `<DspCore.h>` or a single module
  header such as `<DspCore_Core.h>` is enough for the Arduino builder to find
  both libraries. The example includes only the module it demonstrates.

### Added

- 0.1.0 scaffold: RoModularBuild, native, AVR and Arm presets, Bash and
  PowerShell scripts, Foundation `1.4.0` dependency resolution, installable
  CMake package with a consumer test, Arduino library layout, Doxygen
  documentation and CI (native, embedded, Arduino, documentation, sanitizers
  and clang-tidy).
- `DspCore::Core::Version()` and `FoundationVersion()`.
