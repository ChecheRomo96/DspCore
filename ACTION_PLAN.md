# DspCore Action Plan

## Objective

Build DspCore as a portable C++17 signal-processing library for desktop and
direct-source embedded targets, with C++11 Arduino source compatibility:
lookup tables, interpolation, generators, streaming filters, analysis and transforms.
It consumes Foundation for general utilities, has no MIDI or music-theory
dependencies. It never throws exceptions, and memory use is the implementer's
choice.

The dependency direction is:

```text
Foundation <- DspCore <- MIDILAR
```

The legacy `DspCore` module inside MIDILAR (`src/DspCore` at MIDILAR commit `bc70ce6`)
is design input only; nothing is copied without review and tests.

## Architectural decisions

- Same standards as Foundation and MCC: RoModularBuild presets and scripts,
  Bash and PowerShell parity, warnings as errors, sanitizers, clang-tidy,
  installable CMake package, Arduino library layout and Doxygen.
- General utilities (buffers, time, scheduling, flash data) come from
  Foundation; MIDI stays in MIDILAR and music theory in MCC.
- Value types are trivially copyable, `constexpr` where practical, with
  compile-time size budgets for every target. Polymorphic processor nodes have
  separate size budgets and are intentionally not value types.
- Sample buffers are caller-provided.

## Phase 0 - Baseline

Status: complete

- [x] RoModularBuild presets, scripts and CI for native, AVR and Arm.
- [x] Resolve Foundation `1.4.0` (package or sources).
- [x] Installable CMake package, consumer test and Arduino library layout.
- [x] Core module with DspCore and Foundation versions.

## Phase 1 - Numeric and buffer specification

Status: implemented; validation recorded in docs/Design/NumericPolicyProposal.md.
User authorized this phase after approving the architecture and legacy review.

- [x] Review historical algorithms and document defects before migration.
- [x] Specify sample precision, normalized ranges and PCM conversion semantics.
- [x] Specify allocation, overflow, empty shapes, moves and overlap contracts.
- [x] Keep control systems outside DspCore and musical interpretation in MIDILAR.
- [x] Publish Doxygen architecture and numeric/buffer topics.
- [x] Add target-independent precision assertions to the library build.

The built-in float/double types remain the scalar interface; no wrapper or
fixed-point arithmetic type is introduced. Documentation requirements for
future components do not claim those components are already implemented.

## Phase 2 - Lookup table storage

Status: complete. User authorized continuation after phase 1.

- [x] Move-only LUT1D, LUT2D and LUT3D with common storage semantics.
- [x] Checked dimension/byte products and non-throwing allocation failure.
- [x] Reserve, zero-initialized growth, checked access and explicit release.
- [x] Remove virtual generation and dangling-pointer behavior from LUT3D.
- [x] Regression tests, forced allocation failure and installed-package consumer.
- [x] Three shared Arduino/desktop examples and Doxygen topic.

Validation and differences from the legacy API are in docs/Design/LutMigration.md.

## Phase 3 - Interpolation and lookup boundaries

Status: complete. User authorized continuation after phase 2.

- [x] Float/double linear, bilinear and trilinear arithmetic without hidden clamp.
- [x] Checked physical coordinates reject collapsed/non-finite/unrepresentable spans.
- [x] Explicit Clamp/Wrap/Reject LUT lookup, singleton axes and periodic seams.
- [x] Precision, asymmetric field, endpoint, invalid-input and narrowing tests.
- [x] Two shared Arduino/desktop examples and Doxygen topic.
- [x] Installed-package consumer and AddressSanitizer lookup probe.

Validation and API semantics are in docs/Design/InterpolationMigration.md.

## Phase 4 - Generators

Status: complete. User authorized this phase with "next" after phase 3.

- [x] Periodic sine/saw/square/triangle tables with documented historical phases.
- [x] Atan/tanh raw, normalized and half curves; gamma and gamma morph.
- [x] Linear/exp/log envelope segments with stable small-curve limits.
- [x] Optional LUT-shaped segments with checked shape storage and constant singleton shapes.
- [x] Explicit nonzero xorshift state and symmetric Hann/Hamming windows.
- [x] Float/double, domain/range validation, no allocation and checked buffer sizes.
- [x] Reference/regression tests, six Arduino/desktop examples and family topics.
- [x] Installed-package consumer and AddressSanitizer buffer/canary probe.

Semantics and validation are recorded in docs/Design/GeneratorsMigration.md.

## Phase 5 - Streaming filters and analysis

Status: implemented and validated. Control, including Smoother, is outside this repository.

- [x] Runtime-tap FIR with CPSTL vector coefficients/history; Configure may
  resize, while Process does not allocate.
- [x] Runtime-order IIR with CPSTL vector coefficients/histories, independent
  numerator/denominator lengths and a0 normalization.
- [x] Biquad::LowPass/HighPass return a six-scalar Coefficients<T> struct by value.
- [x] Block peak/RMS with scaled RMS arithmetic and finite-input validation.
- [x] Rectified one-pole follower with independent attack/release seconds.
- [x] Size budgets, reference/response tests and five portable examples.
- [x] Package consumer, Arduino compilation, Doxygen and AddressSanitizer probe.

See docs/Design/FiltersAnalysisImplementation.md for contracts and evidence.


## Phase 6 - Processing interfaces

Status: started. User authorized the base interface on 2026-10-20.

- [x] Define ProcessorBase: a MIMO, block-oriented abstract interface with
  non-owning input/output buffers, synchronous n-to-n block cadence, and
  explicit failure, ownership and lifecycle contracts.
- [x] Make FIR and IIR SISO ProcessorBase nodes with n-to-n block-processing
  APIs, rejecting malformed blocks before processing
  and reporting a valid prefix on finite arithmetic failure.
- [ ] Define graph construction, port formats, scheduling, latency, buffer planning
  and audio/control/spectral adapters after concrete consumers are approved.

This phase intentionally does not add a graph, an audio backend, DSP Designer
or implicit allocation. Those remain independent consumers of DspCore.

## Subsequent phases

Each phase needs the user's approval before it starts.

7. Transforms (DFT/IDFT, FFT/IFFT).
8. MIDILAR integration and the first DSP feature release.
