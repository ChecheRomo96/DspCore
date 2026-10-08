# Changelog

This file records user-visible changes to DspCore. Release dates use the
`YYYY-MM-DD` format.

## [Unreleased]

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
