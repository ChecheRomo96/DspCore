# Legacy DSP review

Reviewed 2026-10-16. Source: MIDILAR commit `bc70ce639f8bb72f79995e1ec742b73aef544112`, `src/DspCore`.

This is a review and migration recommendation, not approval of an implementation phase. Historical source remains unchanged under `build/legacy-audit/bc70ce6`; nothing was restored into the standalone public API. The numeric-policy proposal remains a draft.

## Findings requiring correction before reuse

1. **[P1] LUT3D keeps a freed pointer after resizing to an empty shape.** `LUT/LUT3D/LUT3D.tpp:20-26` frees `_buffer` without clearing it. Allocating 2x2x2, then setting 0x0x0 leaves `GetBuffer()` non-null; destruction or a later resize frees the allocation again. The destructor's second free follows directly from lines 13-16. The runtime probe verified the dangling pointer without deliberately executing the double-free. Clear the pointer and define empty-shape evaluation behavior before migration.

2. **[P1] LUT size arithmetic can wrap before allocation.** `LUT/LUT2D/LUT2D.tpp:137` multiplies dimensions without checks; allocation also multiplies element count by `sizeof(T)`. LUT1D and LUT3D similarly lack byte-count checks. On this 64-bit build, `Resize(9223372036854775808, 2)` returned true with nonzero dimensions, `Size()==0`, and no storage. Subsequent accesses can target invalid storage. Check each multiplication before changing state or allocating, and preserve the old object on failure.

3. **[P2] Valid small positive envelope curves produce NaNs.** `Generators/Envelopes/ExpSegment.h:51-57` subtracts nearly equal floats; `LogSegment.h:51-55` rounds `1 + Curve` to one. Both returned non-finite samples for Curve=1e-9, Start=0, End=1, length=3. Use stable `expm1`/`log1p` formulations and specify the linear limit, including exact endpoints.

4. **[P2] GammaMorphLUT3D cannot be instantiated.** `Generators/Shaping/GammaMorph/GammaMorph.h:91,120` calls protected `LUT3D::GetBuffer()` from a free function. MSVC reports C2248 at both calls. The 3D container should expose an intentional storage interface consistent with 1D/2D, rather than requiring a virtual generator to own evaluation.

5. **[P2] Zero noise state never generates noise.** `Generators/Noise/White.h:39-45` uses xorshift32 without excluding zero. An explicit seed of zero produced `[-1,-1,-1]` at default amplitude/offset and remains zero. Reject or replace zero, or document a nonzero precondition. Calls without a supplied state also restart the same sequence; document this distinction from streaming noise.

6. **[P2] Window generation depends on an external constant.** `Generators/Windowing/Hann.h:23` and Hamming use `TWO_PI` without defining it locally. Instantiating Hann with an isolated build-settings header failed with C2065. This is evidence of a portability dependency, not a full historical-build failure: an Arduino or umbrella include may supply the macro. Replace that dependency with a library constant and compile each public header independently.

## What exists and what is worth retaining

| Area | Historical implementation | Recommendation |
| --- | --- | --- |
| LUT1D / LUT2D | Move-only owning arrays, reserve/resize, raw data access, generator wrappers | Retain the storage/generation separation and reserve behavior; harden allocation arithmetic and constrain supported element types. |
| LUT3D | Owning array with virtual `Eval`, protected buffer, no capacity interface matching 1D/2D | Redesign to match the other containers; do not port unchanged. |
| Interpolation | Linear, bilinear, trilinear | Retain formulas; define collapsed-axis behavior. Bilinear/trilinear divide by zero for equal coordinate bounds, unlike Linear's explicit guard. |
| Periodic tables | Sine, saw, square, triangle | Useful table builders. They are not streaming oscillators or band-limited generators. Preserve phase conventions explicitly in examples. |
| Shaping | Atan and tanh raw/normalized/half variants, gamma, gamma morph, 1D/2D/3D wrappers | Retain useful curve families and mathematical documentation after numeric and compile fixes. |
| Envelopes | Linear, exponential, logarithmic and LUT-driven segments | Retain segment/subrange APIs; stabilize curves and specify singleton behavior. The one-entry LUT branch only writes the first output sample. |
| Noise / windows | xorshift white noise, Hann, Hamming | Retain with explicit state and portable constants. |
| Transforms | DFT, IDFT, radix-2 forward FFT | Useful correctness references. Establish normalization, valid sizes and overlap contracts before optimizing or publishing. No inverse FFT implementation was found. |
| Streaming filters | Polymorphic streaming base; zero-byte `Streaming/Filters/IIR.h` | Interface sketch only; concrete IIR processing is not implemented. |

## Numeric and API implications

Templating storage on `T` does not make computation generic: interpolation returns float, most generators compute in float, and transforms accumulate in double but cast into output types (FFT at each stage). Double storage therefore does not imply double-precision computation. Integer output has no common rounding, clipping or saturation rule; bipolar curves converted into unsigned types can exceed representable ranges. This argues for explicitly separating sample storage, calculation precision and conversion policy in the new design. It does not establish a requirement to make all processing float-only.

LUT storage uses raw malloc/free and assignment; unconstrained templates should not imply support for arbitrary nontrivial objects. Define numeric/trivial element requirements and initialization behavior.

DFT/FFT accept caller-owned input/output pointers but do not establish an overlap contract. Their copy/write order cannot generally support aliased input/output; FFT bit-reversal copying is a particular hazard. Specify separate-buffer processing or implement a real in-place path. FFT recomputes trigonometric twiddles inside butterflies; keep it as a reference until measurement justifies optimization.

## Documentation, examples and coverage

The historical tree includes Doxygen formulas and worked Arduino/desktop examples, especially LUT ownership, reserve/resize, raw data, manual oscillators and shaping. These are valuable migration material and should be ported alongside each approved component, updating namespaces and expected output. Examples are demonstrations, not evidence of memory safety or numerical accuracy.

`tests/dspCore/CMakeLists.txt` is empty and no DSP test source exists in the reviewed test tree. The current standalone DspCore baseline passes 3/3 scaffold tests; those do not validate legacy DSP algorithms.

## Verification and limits

An isolated MSVC C++17 harness compiled the unchanged historical component headers with an empty `MIDILAR_BuildSettings.h` stub to avoid loading unrelated old framework code. Artifacts under ignored `build/legacy-audit/`:

- `probe.cpp`, `probe-results.txt`: reproduced findings 1 (dangling-pointer state), 2, 3 and 5.
- `compile-gamma.cpp`, `compile-gamma.log`: reproduced protected-access errors.
- `compile-hann.cpp`, `compile-hann.log`: reproduced missing constant on instantiation.
- Historical source archive and extracted snapshot preserve the exact reviewed implementation.

These are targeted probes and source inspection, not a full rebuild, exhaustive numerical verification, benchmark, sanitizer run or hardware execution. No production implementation was changed.

## Recommended migration sequence

First settle the numeric and buffer contracts using this audit. Then migrate safe LUT storage and interpolation with checks for overflow, allocation failure, moves, empty shapes and degenerate axes. Port generators together with their formulas, examples and reference-value/endpoint checks. Bring transforms across with explicit overlap and normalization contracts and DFT-versus-FFT comparisons. Treat streaming filters as new work rather than recovered functionality.

Each implementation phase remains subject to the approval rule in `AGENTS.md` and `ACTION_PLAN.md`.
