# Control implementation

Phase 5 adds DspCore::Control for signal/system processing. User authorized
this next step after committing/pushing the preceding DSP work. The Control
implementation remains local for review; it is not part of the published
phase 1-4 commit or the documentation-index/CI repair commits.

## API contracts

- Smoother: one-pole y=lerp(y,input,1-exp(-dt/tau)), stable for tiny dt/tau;
  zero tau bypasses smoothing. Configure preserves value, Reset sets it.
- LinearRamp: explicit target/duration, retarget from current value, exact
  arrival, snap for zero duration. Reset cancels progression.
- SlewLimiter: separate nonnegative rise/fall rates in value units per second;
  a zero rate holds that direction. Reachable targets snap exactly.
- Map: ascending finite input bounds, optional inverted/constant output,
  explicit Clamp/Reject/Extrapolate policies. All results must remain finite.
- Deadband: continuous zero zone, subtract width outside it. Hysteresis:
  inclusive lower/upper thresholds, latched state and explicit reset.
- PID: nonnegative gains, derivative on measurement, optional derivative
  one-pole filter, output limits, conditional integration plus integral-term
  clamping. The derivative has no setpoint kick or first-measurement impulse.
  Reset clears history; Configure clamps stored integral/output to new limits
  without claiming bumpless gain changes. There are no automatic tuning or
  actuator-specific assumptions. Plant feedback polarity belongs to callers.
- Updates use finite positive seconds; invalid descriptors, sample domains or
  arithmetic leave state/output unchanged. Zero dt is rejected, never silently
  skipped. No hidden scheduler, wall clock, allocation or MIDI interpretation.
- Float/double value objects are trivially copyable with target-compiled size
  budgets. Copying duplicates dynamic state; instances then evolve independently.
- SPEC-CTL-2..6 and the complete PID equations are published in Control.dox.

## Validation

- Windows MSVC: 63/63 tests pass (44 previous, 10 control primitives, 9 PID).
- Tests include analytic one-pole response, interval partition equivalence,
  tiny intervals, ramp retargeting, asymmetric rates/exact snapping, mapping
  policies, threshold latching, double precision and transactional failures.
- PID tests include P/I units, saturation/recovery, no derivative kick, filtered
  derivative reference, reset/reconfiguration, arithmetic rejection and a
  simulated first-order plant converging to its target.
- Five native examples execute and print actual/expected outputs with PASS.
- Seventeen Arduino Uno examples compile in stock C++11, including five control
  examples. This proves compilation/linking, not physical-board execution or
  measured sample rates. State size assertions are included in the library build.
- Installed Release package consumer exercises smoothing, ramp and PID.
- Local Doxygen renders the Control topics and all public classes/descriptions.
  The existing obsolete DOT_MULTI_TARGETS warning remains with local Doxygen 1.18.
- The simulated plant is a numerical regression test, not a hardware stability
  or application-level tuning guarantee. Arm/control performance is unmeasured.

The hosted documentation request was handled separately: GitHub Pages was
configured to publish the docs branch, a rebuild requested, and the deploy
workflow repaired to request subsequent Pages builds automatically. Hosted
Topics and class-index discovery fixes were verified by HTTP before opening.
