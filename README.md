# DspCore

DspCore is the signal-processing layer of the RoModular ecosystem: lookup
tables, interpolation, generators, filters and transforms for desktop and
embedded targets. It is built on Foundation, has no MIDI or music-theory
dependencies. It never throws exceptions, and memory use is the
implementer's choice.

> **Status: early development.** Version 0.1.0 is the scaffold: build,
> packaging, CI and version information only. Features arrive in the phases of
> `ACTION_PLAN.md`.

Clone with the pinned build infrastructure:

```bash
git clone --recurse-submodules https://github.com/ChecheRomo96/DspCore.git
```

For an existing checkout:

```bash
git submodule update --init --recursive
```

## Dependencies

DspCore links `Foundation::Foundation` (Foundation `1.4.0` or a newer `1.x`).
Configuring resolves it from a parent project, an explicit prefix
(`DSPCORE_FOUNDATION_PREFIX`), a sibling export in `../Foundation/dist/<preset>`,
normal `find_package`, and finally the pinned GitHub Release package, or the
sources at that tag for AVR and Arm presets.

## Build and test

The presets and scripts are the supported interface. Use the matching
PowerShell script on Windows.

```bash
./scripts/test.sh macos_arm64 --config Debug
./scripts/test.sh macos_arm64 --config Release
./scripts/build.sh macos_arm64 --config Release --examples-on
./scripts/test-package.sh macos_arm64
./scripts/export.sh atmega328p_avrgcc_avr5
./scripts/test-arduino.sh
./scripts/analyze.sh macos_arm64
./scripts/docs.sh
```

The generated documentation starts at
`build/documentation/docs/html/index.html`.

## Arduino

Install Foundation and DspCore as Arduino libraries, then include DspCore, or
only the modules the sketch uses. Every DspCore header also brings in
Foundation, so the Arduino builder finds both libraries:

```cpp
#include <DspCore.h>       // every module
#include <DspCore_Core.h>  // or only the core module
```

DspCore requires C++17. On the stock Arduino AVR core add `-std=gnu++17`, for
example with
`arduino-cli compile --build-property "compiler.cpp.extra_flags=-std=gnu++17"`.

## License

Copyright (c) 2026 José Manuel Romo. All rights reserved.

DspCore is currently proprietary. No permission is granted for external use,
compilation, modification, redistribution, integration, or commercial use
without prior written authorization. See [LICENSE](LICENSE).
