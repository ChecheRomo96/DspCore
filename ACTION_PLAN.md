# DspCore Action Plan

## Objective

Build DspCore as a portable C++17 signal-processing library for desktop and
embedded targets: lookup tables, interpolation, generators, streaming filters
and transforms. It consumes Foundation for general utilities, has no MIDI or
music-theory dependencies, and never allocates memory or throws exceptions in
real-time paths.

The dependency direction is:

```text
Foundation <- DspCore <- MIDILAR
```

The legacy `DspCore` module inside MIDILAR (`src/DspCore` on MIDILAR `main`)
is design input only; nothing is copied without review and tests.

## Architectural decisions

- Same standards as Foundation and MCC: RoModularBuild presets and scripts,
  Bash and PowerShell parity, warnings as errors, sanitizers, clang-tidy,
  installable CMake package, Arduino library layout and Doxygen.
- General utilities (buffers, time, scheduling, flash data) come from
  Foundation; MIDI stays in MIDILAR and music theory in MCC.
- Value types are trivially copyable, `constexpr` where practical, with
  compile-time size budgets for every target.
- Sample buffers are caller-provided.

## Phase 0 - Baseline

Status: complete

- [x] RoModularBuild presets, scripts and CI for native, AVR and Arm.
- [x] Resolve Foundation `1.4.0` (package or sources).
- [x] Installable CMake package, consumer test and Arduino library layout.
- [x] Core module with DspCore and Foundation versions.

## Proposed phases

Each phase needs the user's approval before it starts.

1. Numeric specification: sample types, fixed-point and floating-point
   policy per target.
2. Lookup tables (1D, then 2D and 3D).
3. Interpolators (linear, bilinear, trilinear).
4. Generators (periodic, shaping, envelopes, noise, windowing).
5. Streaming filters.
6. Transforms.
7. MIDILAR integration and the first tagged release.
