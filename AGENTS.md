# DspCore agent instructions

DspCore is the signal-processing layer of the RoModular ecosystem: lookup
tables, interpolation, generators, filters and transforms. Keep it
independently buildable for desktop and embedded consumers, and preserve the
dependency direction `Foundation <- DspCore <- MIDILAR`.

Follow `ACTION_PLAN.md` phase by phase; each phase needs the user's approval.

## Scope

Do not modify Foundation, MCC, RoModular, RoModularBuild, or another sibling
repository unless the user explicitly includes it.

## Repository rules

- Treat `CMakePresets.json`, its included preset files, and the scripts under
  `scripts/` as the supported build interface, with Bash and PowerShell
  parity.
- Initialize the pinned `tools/RoModularBuild` submodule before invoking a
  workflow in a fresh checkout, and treat it as read-only.
- Foundation is resolved by `cmake/DspCoreFoundation.cmake`; keep its pinned
  version in step with the Arduino CI job.
- General utilities belong in Foundation, music theory in MCC and MIDI in
  MIDILAR; only signal processing belongs here.
- Code never throws exceptions. Operations that change a container's
  size may allocate; the library makes no real-time assumptions about the
  caller, and implementers who modify storage in time-critical code are
  expected to reserve the space beforehand. Framework types report allocation
  failure through their result (`bool` or invalid state), never by exceptions
  or undefined behavior.
- Value types are trivially copyable with compile-time size
  budgets in `src/DspCore.cpp`.
- Examples demonstrate public APIs; unit tests live under `tests/DspCore/`
  and use GoogleTest through CTest.
- Keep public headers, examples, tests, version metadata, `ACTION_PLAN.md`,
  `CHANGELOG.md` and Doxygen synchronized with public API changes.
- Separate compile/link validation from hardware execution evidence.
- Preserve unrelated work and do not commit, tag, push, publish, or merge
  unless the user explicitly requests it.

## Supported entry points

Use the PowerShell equivalent on Windows.

```text
./scripts/configure.sh <preset> [--fresh] [-- <cmake-options>]
./scripts/build.sh <preset> [--fresh] [--config <configuration>] [--examples-on]
./scripts/test.sh <preset> [--fresh] [--config <configuration>]
./scripts/install.sh <preset>
./scripts/export.sh <preset> [--fresh] [--examples-on]
./scripts/test-package.sh <preset> [--fresh]
./scripts/test-arduino.sh [--fqbn <board>] [--foundation <dir>]
./scripts/analyze.sh <preset> [--fresh]
./scripts/docs.sh [--fresh]
```
