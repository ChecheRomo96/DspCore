# DspCore numeric policy proposal

Status: superseded by the phase 1 specification in docs/Topics/NumericContracts.dox. The user authorized this phase after the legacy review and architecture agreement.

## Starting point

DspCore 0.1.3 contains the Core/version scaffold, build infrastructure,
packaging, desktop/embedded CI and one portable version example. There are
no implemented lookup tables, interpolation, generators, filters or transforms
in the current standalone source tree.

The historical implementation is available in MIDILAR commit
bc70ce639f8bb72f79995e1ec742b73aef544112 under src/DspCore. It is design input,
as required by AGENTS.md. The first audit covered:

| Historical component | Observation | Consequence for the new design |
| --- | --- | --- |
| LUT/LUT1D/LUT1D.h | Templated storage, dynamic ownership, size and capacity | Separate table access from allocation; preserve caller-provided buffers |
| Interpolators/Linear/Linear.h | Templated input samples, but float coordinates and return value | Storage type and computation precision need explicit, separate contracts |
| Generators/Periodic/Sine/Sine.h | Templated output, float math and unchecked conversion to T | Specify range, rounding and saturation before offering integer output |
| Streaming/Filters/IIR.h | Empty placeholder in the inspected revision | Do not describe filters as implemented |
| Transforms/FFT.h | Existing algorithm helpers and transform code | Review numeric precision and caller workspace before reuse |

The old LUT and generator documentation should be audited with the code and
adapted alongside tested APIs, rather than importing declarations wholesale.

## Proposed policy

1. Distinguish sample storage from processing arithmetic. Signed integer PCM
   storage is not automatically a fixed-point arithmetic type.
2. Start floating-point processing with float as the documented convenience
   default. Allow explicit double where the algorithm supports it. Do not
   select a different public type merely because the target changes.
3. Do not promise that double means 64 bits on every embedded toolchain.
   Record and validate actual numeric_limits precision in platform tests.
4. Keep normalized audio conventionally bipolar (-1 to +1), while permitting
   intermediate headroom. Do not insert hidden clipping in generic processing.
   Unipolar controls/envelopes use a separately documented 0-to-1 convention.
5. PCM conversion must explicitly define rounding, saturation and endpoints.
   Out-of-range floating-to-integer casts and signed overflow are forbidden.
6. Fixed-point processing needs a reviewed arithmetic contract: format,
   intermediate width, multiplication rounding, division and saturation.
   Foundation owns general fixed-point arithmetic; no such scalar facility
   was found in the inspected Foundation source tree. Do not invent one
   inside DspCore merely to complete a generator.
7. Parameter validation and sample propagation are separate. Checked setup
   operations report invalid sample rates, sizes or coefficients without
   exceptions. Each later algorithm must document handling of NaN/Inf,
   denormals and invalid state; no blanket promise is made by this draft.
8. Buffers and transform workspaces belong to the caller. Allocation choices
   must stay explicit; compile success is not a real-time performance claim.
9. LUT coordinates and oscillator phase need separate boundary policies:
   closed/clamped lookup versus periodic wrapping. An interpolator must not
   choose the application's edge behavior implicitly.

## Platform policy to validate

| Target family in existing presets | Proposed initial path | Required evidence |
| --- | --- | --- |
| Desktop | float and explicit double processing | Precision, error bounds and sanitizer tests |
| AVR | Small float-based examples; explicit integer storage conversions | Actual scalar precision, memory budget and measured execution before audio-rate claims |
| Cortex-M0+/M3 soft-float profiles | float reference algorithms | Compile/package checks first; throughput measured on hardware |
| Cortex-M4F/M7F hard-float profiles | Same public types; target-selected optimization later | Reference equivalence, ABI and hardware measurements |

Fixed-point kernels and target-specific acceleration remain separate choices
to approve after this baseline; they are not silently substituted for float.

## Recorded decisions

Float is the convenience default; explicit double must preserve calculation
precision. Fixed-point kernels remain a separate Foundation prerequisite.
Initial migration targets LUTs/interpolation useful for both control and audio;
no hardware audio-rate performance claim is made. Control systems are outside DspCore; musical MIDI control stays in MIDILAR.

The normative invariant identifiers are SPEC-NUM-1..6, SPEC-BUF-1..6,
SPEC-DOM-1..2 and SPEC-CTL-1 in docs/Topics/NumericContracts.dox.

## Validation

- `scripts/test.ps1 windows_msvc_x64`: build succeeded and 3/3 scaffold tests
  passed; the new precision assertions compiled.
- `scripts/build.ps1 atmega328p_avrgcc_avr5`: AVR-GCC 7.3 compiled the
  library and precision assertions successfully using the installed Arduino
  toolchain. This is cross-compilation evidence, not board execution.
- `scripts/docs.ps1`: Doxygen generated both new linked topics. The existing
  configuration emits an obsolete DOT_MULTI_TARGETS warning with Doxygen 1.18.
- Arm preset configuration could not find arm-none-eabi-gcc in this environment;
  Arm verification remains pending in CI or an equipped environment.
- No hardware measurements were performed. Historical probes are documented
  separately in LegacyDspReview.md and do not validate future algorithms.
