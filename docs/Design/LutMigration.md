# LUT storage migration

Phase 2 implements `DspCore::LUT::LUT1D<T>`, `LUT2D<T>` and `LUT3D<T>`.
The pre-rebuild MIDILAR source is design input, not source compatibility.

## API decisions

- Include DspCore_LUT.h, DspCore/LUT.h, or an individual dimension header.
- Owning tables accept mutable arithmetic types excluding bool. No copy;
  noexcept moves transfer ownership and empty every source extent.
- Resize uses element dimensions; Reserve accepts flat element capacity.
- Fill, Size, Capacity, Empty, GetBuffer and Release are common to all ranks.
- Width is available on all ranks, Height on 2D/3D, Depth on 3D.
- SetValue/TryGet provide checked access; GetValue is a zero-fallback convenience.
- Checked arithmetic covers dimensions and bytes before allocation. Failed
  allocation leaves the old pointer, shape, capacity and values intact.
- Growth initializes new logical elements to zero; resizing preserves the
  flat prefix. Any zero extent canonicalizes the empty shape but retains
  capacity. Release, rather than empty resize, frees storage.
- Allocation uses C malloc/free, independent of CPSTL's optional throwing
  standard allocator mode. CPSTL supplies arithmetic traits and portable
  placement construction declarations, including bare AVR.
- 3D storage no longer inherits a virtual generation interface. Generators
  will fill the same public contiguous storage interface as 1D and 2D.
- Raw data can be copied into the first Size elements through GetBuffer;
  no implicit source ownership or hidden allocating import is introduced.
- Interpolation, external-buffer views and generators are not part of this
  phase. Owning resource types are excluded from trivial-value requirements.

See the Doxygen Lookup tables topic for formulas, pointer invalidation,
allocation behavior and examples. SPEC-BUF-1..6 govern these decisions.

## Validation

- Windows MSVC: 12/12 tests pass (3 scaffold, 8 LUT behavior, 1 isolated
  forced-allocation-failure test covering all three ranks).
- Installed Release package: external consumer builds/runs with all three
  LUT headers and transitive Foundation/CPSTL includes.
- Three native examples run and report values, expectations and PASS/FAIL.
- Four Arduino Uno sketches compile in stock C++11 mode (version plus three
  LUT examples). This is compile/link evidence, not board execution; heap
  usage is additional to Arduino's static-memory report.
- MSVC AddressSanitizer standalone probe passed 100 move/empty/release
  cycles and forced allocation failure without reported address errors.
  Probe source/executable are under ignored build/lut-validation.
- Doxygen generates the LUT topic and public types. Its existing obsolete
  DOT_MULTI_TARGETS configuration warning remains unrelated to these APIs.
- No Arm compiler or physical-board execution is available in this session.

No commit, push, version tag or publication was performed for this phase.
